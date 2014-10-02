#include "dr_api.h"
#include "drwrap.h"
#include "drsyms.h"
#include "drmgr.h"
#include <string.h>

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

static void
event_thread_init(void* drcontext)
{
    thread_info_t* thread_info = dr_thread_alloc(drcontext, sizeof(thread_info_t));
    /*thread_info->lock = dr_thread_alloc(drcontext, sizeof(void*)*MAX_LOCKS);*/

    drmgr_set_tls_field(drcontext, tls_index, thread_info);

    dr_mutex_lock(num_threads_lock);
    thread_info->tid = num_threads;
    num_threads++;
    dr_mutex_unlock(num_threads_lock);

    thread_info->sbag = dr_global_alloc(sizeof(drvector_t));
    thread_info->pbag = dr_global_alloc(sizeof(drvector_t));

    drvector_init(thread_info->sbag, 10, false /*synch*/, NULL);
    drvector_init(thread_info->pbag, 10, false /*synch*/, NULL);

    drvector_append(thread_info->sbag, &thread_info->tid);

    if (thread_info->tid == 0)
    {
        main_sbag = thread_info->sbag;
        main_pbag = thread_info->pbag;
    }

    thread_info->num_locks = 0;
}

static void
event_thread_exit(void* drcontext)
{
    thread_info_t* thread_info = (thread_info_t*)drmgr_get_tls_field(drcontext, tls_index);
    dr_printf("Total locks held from thread #%d : %d\n", thread_info->tid, thread_info->num_locks);
    int tid = thread_info->tid;

    int i;
    drvector_t* sbag = thread_info->sbag;

    for(i=0; i<sbag->entries; i++)
    {
        drvector_append(main_pbag, drvector_get_entry(sbag, i));
    }

    drvector_delete(thread_info->sbag);
    drvector_delete(thread_info->pbag);

    dr_thread_free(drcontext, thread_info, sizeof(thread_info_t));
}

/* Checks weather mem ref is in stack frame aka is local */
static bool
opnd_is_stack_addr(void* drcontext, opnd_t opnd)
{
    dr_mcontext_t mcontext;
    mcontext.flags = DR_MC_CONTROL | DR_MC_INTEGER;
    mcontext.size = sizeof(mcontext);

    dr_get_mcontext(drcontext, &mcontext);

    app_pc addr = opnd_compute_address(opnd, &mcontext);

    return addr <= (app_pc)mcontext.xsp && addr >= (app_pc)mcontext.xbp;
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
    if (instr_reads_memory(instr)) {
        for (i=0; i<instr_num_srcs(instr); i++) {
            opnd_t opnd = instr_get_src(instr, i);
            if (opnd_is_memory_reference(opnd) && !opnd_is_stack_addr(drcontext, opnd)) {
                /*instrument_mem(drcontext, bb, instr, i, false);*/
                /*dr_printf("[+] global memory access!\n");*/
            }
        }
    }
    if (instr_writes_memory(instr)) {
        for (i=0; i<instr_num_dsts(instr); i++) {
            opnd_t opnd = instr_get_dst(instr, i);
            if (opnd_is_memory_reference(opnd) && !opnd_is_stack_addr(drcontext, opnd)) {
                /*dr_printf("[+] global memory access!\n");*/
                /*instrument_mem(drcontext, bb, instr, i, true);*/
            }
        }
    }
    return DR_EMIT_DEFAULT;
}

static void
event_exit(void)
{
    drwrap_exit();
    drsym_exit();
    drmgr_exit();
    /*umbra_exit();*/
    dr_mutex_destroy(num_threads_lock);
    dr_mutex_destroy(malloc_table_lock);
    dr_mutex_destroy(runlock);
}


DR_EXPORT void
dr_init(client_id_t id)
{
    /*umbra_init(id);*/
    drwrap_init();
    drmgr_init();
    drsym_init(0);

    drmgr_register_module_load_event(event_module_load);
    drmgr_register_thread_init_event(event_thread_init);
    drmgr_register_thread_exit_event(event_thread_exit);
    drmgr_register_bb_instrumentation_event(NULL, event_bb_insert, NULL);

    dr_register_exit_event(event_exit);

    num_threads_lock = dr_mutex_create();
    malloc_table_lock = dr_mutex_create();
    runlock = dr_mutex_create();

    tls_index = drmgr_register_tls_field();
}
