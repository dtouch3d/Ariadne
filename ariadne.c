#include "dr_api.h"
#include "drwrap.h"
#include "drsyms.h"
#include "drmgr.h"
#include "umbra.h"
#include <string.h>
#include <stdint.h>

#include "ariadne.h"

void* runlock;
static int running_thread = 0;

static void
event_module_load(void *drcontext, const module_data_t *info, bool loaded)
{
    int i,j;
    const char* modname = dr_module_preferred_name(info);
    module_handle_t modhandle = info->handle;

    /* We check if a library we want to instrument is loaded from
     * the global modtable array at ariadne.h */
    for(i=0; i<SIZE(modtable); i++)
    {
        if(strstr(modname, modtable[i]) != NULL)
        {
            /*dr_printf("[+] Loaded %s module\n", modname);*/
            for(j=0; j<SIZE(symtab); j++)
            {
                symtab_t func = symtab[j];
                void* addr = dr_get_proc_address(modhandle, func.name);
                /*dr_printf("[+] Found %s @ %p\n", func.name, addr);*/
                int index = findfunc(func.name);

                if (index == -1)
                    return;

                void (*pre_func)() = symtab[index].pre_func_cb;
                void (*post_func)() = symtab[index].post_func_cb;
                /*dr_printf("[+] %s handler is at %p\n", func.name, funcp);*/

                /* Calls funcp before actual call */
                drwrap_wrap((app_pc)addr, pre_func, post_func);
            }
        }
    }
}

/* We use 1 byte per 4 bytes of memory, indicating the thread of the last
 * access to memory.
 */
#define SHADOW_GRANULARITY 4
#define SHADOW_MAP_SCALE   UMBRA_MAP_SCALE_DOWN_4X

#define SHADOW_DEFAULT_VALUE      19
#define SHADOW_DEFAULT_VALUE_SIZE 1

uint
shadow_get_byte(app_pc addr)
{
    byte val;
    size_t app_size = SHADOW_GRANULARITY;
    size_t shdw_size = sizeof(val);
    int res =umbra_read_shadow_memory(umbra_map, addr, app_size, &shdw_size, &val);

    if (res != DRMF_SUCCESS || shdw_size != sizeof(val))
    {
        dr_printf("[!] failed to get shadow byte of %p : %d\n", addr, res);
    }
    dr_printf("[!] (get) shdw_size: %d, sizeof(val): %d\n", shdw_size, sizeof(val));
    return val;
}

void
shadow_set_byte(app_pc addr, byte val)
{
    size_t app_size = SHADOW_GRANULARITY;
    size_t shdw_size = sizeof(val);
    int res = umbra_write_shadow_memory(umbra_map, addr, app_size, &shdw_size, &val);
    if (res != DRMF_SUCCESS || shdw_size != sizeof(val))
    {
        dr_printf("[!] failed to set shadow byte of %p : %d\n", addr, res);
    }
    dr_printf("[!] (set) shdw_size: %d, sizeof(val): %d\n", shdw_size, sizeof(val));
}

static void
event_thread_init(void* drcontext)
{
    thread_info_t* thread_info = dr_global_alloc(sizeof(thread_info_t));

    drvector_append(thread_info_vec, thread_info);

    unsigned int* tid = dr_thread_alloc(drcontext, sizeof(unsigned int));

    dr_mutex_lock(num_threads_lock);
    thread_info->tid = num_threads;
    num_threads++;
    dr_mutex_unlock(num_threads_lock);

    *tid = thread_info->tid;

    drmgr_set_tls_field(drcontext, tls_index, tid);

    thread_info->sbag = dr_global_alloc(sizeof(drvector_t));
    thread_info->pbag = dr_global_alloc(sizeof(drvector_t));

    drvector_init(thread_info->sbag, 16, true /*synch*/, NULL);
    drvector_init(thread_info->pbag, 16, true /*synch*/, NULL);

    drvector_append(thread_info->sbag, (void*)*tid);

    thread_info->num_locks = 0;
}

static void
event_thread_exit(void* drcontext)
{
    thread_info_t* thread_info = get_thread_info_helper(drcontext, true);

    unsigned int tid = thread_info->tid;

    dr_printf("Total locks held from thread #%d : %d\n", tid, thread_info->num_locks);

    int i;
    drvector_t* sbag = thread_info->sbag;
    drvector_t* pbag = thread_info->pbag;

    if (tid > 0)
    {
        /* Here we join the sbag of spawned thread with it's parent's pbag.
         * For now we assume no nested parallelism */
        /*dr_printf("entries: %d\n", sbag->entries);*/
        for(i=0; i<sbag->entries; i++)
        {
            /*dr_printf("appending %d\n", i);*/
            thread_info_t* main_info = drvector_get_entry(thread_info_vec, 0);
            /*dr_printf("main thread bags #%p  and %p\n", main_info->sbag, main_info->pbag);*/
            drvector_append(main_info->pbag, drvector_get_entry(sbag, i));
        }

    } else /* main thread */
    {
        dr_printf("[+] sbag:");
        print_bag(sbag);

        dr_printf("[+] pbag:");
        print_bag(pbag);
    }
}

app_pc
opnd_calc_address(void* drcontext, opnd_t opnd)
{
    dr_mcontext_t mcontext;
    mcontext.flags = DR_MC_CONTROL | DR_MC_INTEGER;
    mcontext.size = sizeof(mcontext);

    dr_get_mcontext(drcontext, &mcontext);

    return opnd_compute_address(opnd, &mcontext);
}

void* clean_call()
{
    dr_printf("[+] in instrumented memory!\n");
    return NULL;
}

/* Called for every instr on bb */
/* TODO: check if in main module */
static dr_emit_flags_t
event_bb_insert(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                bool for_trace, bool translating, void *user_data)
{
    int i;
    if (instr_get_app_pc(instr) == NULL)
        return DR_EMIT_DEFAULT;
    if (instr_reads_memory(instr))
    {
        for (i=0; i<instr_num_srcs(instr); i++)
        {
            opnd_t opnd = instr_get_src(instr, i);
            if (opnd_is_memory_reference(opnd))
            {
                /*print_malloc_chunks();*/
                app_pc addr_read = opnd_calc_address(drcontext, opnd);
                if(in_malloc_chunk(addr_read))
                {
                    dr_insert_clean_call(drcontext, bb, instr, clean_call, false, 0);
                }
            }
        }
    }
    if (instr_writes_memory(instr)) {
        for (i=0; i<instr_num_dsts(instr); i++) {
            opnd_t opnd = instr_get_dst(instr, i);
            if (opnd_is_memory_reference(opnd))
            {
                app_pc addr_write = opnd_calc_address(drcontext, opnd);
                if(in_malloc_chunk(addr_write))
                {
                    if(in_malloc_chunk(addr_write))
                    {
                        dr_insert_clean_call(drcontext, bb, instr, clean_call, false, 0);
                    }
                }
            }
        }
    }
    return DR_EMIT_DEFAULT;
}

static void
shadow_memory_init(void)
{
    umbra_map_options_t umbra_map_ops;

    memset(&umbra_map_ops, 0, sizeof(umbra_map_ops));
    umbra_map_ops.struct_size = sizeof(umbra_map_ops);
    umbra_map_ops.flags = UMBRA_MAP_CREATE_SHADOW_ON_TOUCH;
    umbra_map_ops.scale = SHADOW_MAP_SCALE;
    umbra_map_ops.default_value = SHADOW_DEFAULT_VALUE;
    umbra_map_ops.default_value_size = SHADOW_DEFAULT_VALUE_SIZE;

    if (umbra_create_mapping(&umbra_map_ops, &umbra_map) != DRMF_SUCCESS)
        dr_printf("[!] fail to create shadow memory mapping");
}

void
shadow_memory_destroy(void)
{
    if (umbra_destroy_mapping(umbra_map) != DRMF_SUCCESS)
        dr_printf("[!] fail to destroy shadow memory");
}

static void
event_exit(void)
{
    drwrap_exit();
    drmgr_exit();
    drsym_exit();
    shadow_memory_destroy();
    dr_mutex_destroy(num_threads_lock);
    dr_mutex_destroy(malloc_table_lock);
    dr_mutex_destroy(runlock);

    /* TODO: Free properly */
    /*dr_global_free(thread_info_vec, sizeof(drvector_t));*/

    if (umbra_exit() != DRMF_SUCCESS)
        dr_printf("[!] Umbra exit error!\n");
}


DR_EXPORT void
dr_init(client_id_t id)
{
    drmgr_init();
    umbra_init(id);
    drwrap_init();
    drsym_init(0);

    drmgr_register_module_load_event(event_module_load);
    drmgr_register_thread_init_event(event_thread_init);
    drmgr_register_thread_exit_event(event_thread_exit);
    drmgr_register_bb_instrumentation_event(NULL, event_bb_insert, NULL);

    dr_register_exit_event(event_exit);

    shadow_memory_init();

    num_threads_lock = dr_mutex_create();
    malloc_table_lock = dr_mutex_create();
    runlock = dr_mutex_create();

    tls_index = drmgr_register_tls_field();

    thread_info_vec = dr_global_alloc(sizeof(drvector_t));
    drvector_init(thread_info_vec, 16, true /* synch */, NULL);

    memset(malloc_table, 0, MAX_CHUNKS*sizeof(malloc_chunk_t));
}
