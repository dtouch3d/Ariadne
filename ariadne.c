#include "dr_api.h"
#include "drwrap.h"
#include "drsyms.h"
#include "drmgr.h"
#include "umbra.h"
#include "drutil.h"

#include <string.h>
#include <stdint.h>

#include <stddef.h>
#include "ariadne.h"

void* runlock;
static int running_thread = 0;


/* Checks the memory access for race conditions */

static void
brelly(unsigned int thread, app_pc addr)
{
    dr_mutex_lock(runlock);

    byte shadow_bytes[2];
    shadow_get_byte(addr, shadow_bytes);

    unsigned int accessor = (unsigned int)shadow_bytes[0];
    byte lockset = shadow_bytes[1];

    thread_info_t* main_info = drvector_get_entry(thread_info_vec, 0);
    thread_info_t* accessor_info = drvector_get_entry(thread_info_vec, accessor);
    thread_info_t* thread_info = drvector_get_entry(thread_info_vec, thread);

    dr_printf("[+] addr: %p\n", addr);
    dr_printf("[+] accessor: %d w/ lockset %d\n", accessor_info->tid, lockset);
    dr_printf("[+] current thread: %d w/ lockset %d\n", thread_info->tid, thread_info->lockset);

    int i, j;
    for (i=0; i<main_info->sbag->entries; i++)
    {
        /* serial thread in sbag */
        unsigned int sthread = drvector_get_entry(main_info->sbag, i);
        if (accessor == sthread)
        {
            /* serial access */
            /* ... */


            lockset = thread_info->lockset;

            if (0 /*what*/)
            {
            }

            for (j=0; j<MAX_LOCKS; j++)
            {
                if (lockset & (1 << j))
                {
                    lock[j].alive = 1;
                }
            }

            accessor = thread;

            shadow_bytes[0] = accessor;
            shadow_bytes[1] = lockset;

            shadow_set_byte(addr, shadow_bytes);
            dr_mutex_unlock(runlock);
            return;
        }
    }

     /* parallel access */
    /* ... */

    for (j=0; j<MAX_LOCKS; j++)
    {
        /* if lock belongs to shadow lockset but not in thread's lockset */
        if ((lockset & (1 << j)) && !(thread_info->lockset & (1 << j)))
        {
            if (lock[j].alive)
            {
                lock[j].alive = 0;
                lock[j].nonlocker = thread;
            }
        }
    }

    for (j=0; j<MAX_LOCKS; j++)
    {
        /* if lock belongs to shadow lockset AND in thread's lockset */
        if ((lockset & (1 << j)) && (thread_info->lockset & (1 << j)))
        {
            if (lock[j].alive && in_drvector(main_info->sbag, lock[j].nonlocker))
            {
                lock[j].alive = 0;
            }
        }
    }

    bool all_dead = true;

    for (j=0; j<MAX_LOCKS; j++)
    {
        /* if lock belongs to shadow lockset AND in thread's lockset */
        if ((lockset & (1 << j)))
        {
            if (lock[j].alive)
            {
                all_dead = false;
                break;
            }
        }
    }


    if (lockset == 0 || all_dead)
    {
        /* report race */

        for (j=0; j<MAX_LOCKS; j++)
        {
            /* if lock belongs to shadow lockset AND in thread's lockset */
            if ((lockset & (1 << j)) && (thread_info->lockset & (1 << j)))
            {
                dr_printf("[!] Race error! Thread #%d accesses %p without lock #%d at %p\n",
                        lock[j].nonlocker, addr, j, lock[j].addr);
            }
        }
    }

    dr_mutex_unlock(runlock);
}

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

    thread_info->lockset = 0;
    thread_info->num_locks_held = 0;
}

static void
event_thread_exit(void* drcontext)
{
    thread_info_t* thread_info = get_thread_info_helper(drcontext, true);

    unsigned int tid = thread_info->tid;

    dr_printf("[+] Total locks held from thread #%d : %d\n", tid, thread_info->num_locks_held);

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

void*
clean_call(void* drcontext, app_pc addr)
{
    byte b[2];
    shadow_get_byte(addr, b);
    /*dr_printf("clean call drcontext @ %p\n", drcontext);*/
    /*dr_printf("[+] in instrumented memory @ %p\n", addr);*/
    /*dr_printf("[+]      shadow mem: %d, %d\n", b[0], b[1]);*/

    thread_info_t* thread_info = get_thread_info_helper(drcontext, true);
    brelly((byte)thread_info->tid, addr);
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
                app_pc addr = opnd_calc_address(drcontext, opnd);
                if(in_malloc_chunk(addr))
                {
                    dr_printf("[+] bb_insert: drcontext = %p\n", drcontext);
                    dr_printf("[+] dr_using_private_caches: %d\n", dr_using_all_private_caches());
                    dr_printf("[+] thread id %d\n", dr_get_thread_id(drcontext));
                    dr_insert_clean_call(drcontext, bb, instr, clean_call, false, 2,
                            OPND_CREATE_INTPTR(drcontext), OPND_CREATE_INTPTR(addr));
                }
            }
        }
    }
    if (instr_writes_memory(instr)) {
        for (i=0; i<instr_num_dsts(instr); i++) {
            opnd_t opnd = instr_get_dst(instr, i);
            if (opnd_is_memory_reference(opnd))
            {
                app_pc addr = opnd_calc_address(drcontext, opnd);
                if(in_malloc_chunk(addr))
                {

                    /* XXX: -thred_private alternative
                     * Spill a register to get a pointer to our TLS structure.
                     * dr_save_reg(drcontext, bb, instr, DR_REG_XDI, SPILL_SLOT_2);
                     * dr_insert_read_tls_field(drcontext, bb, instr, DR_REG_XDI);
                     *  ...
                     * dr_restore_reg(drcontext, bb, instr, DR_REG_XDI, SPILL_SLOT_2);
                     */

                    /* XXX: drcontext seems the same for every call
                     * Should be different for every thread so we can take the tls storage
                     */

                    dr_printf("[+] bb_insert: drcontext = %p\n", drcontext);
                    dr_printf("[+] dr_using_private_caches: %d", dr_using_all_private_caches());
                    dr_printf("[+] thread id %d\n", dr_get_thread_id(drcontext));

                    dr_insert_clean_call(drcontext, bb, instr, clean_call, false, 2,
                            OPND_CREATE_INTPTR(drcontext), OPND_CREATE_INTPTR(addr));



                }
            }
        }
    }
    return DR_EMIT_DEFAULT;
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
    dr_mutex_destroy(lock_mutex);

    dr_global_free(thread_info_vec, sizeof(drvector_t));

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
    lock_mutex = dr_mutex_create();

    tls_index = drmgr_register_tls_field();

    thread_info_vec = dr_global_alloc(sizeof(drvector_t));
    drvector_init(thread_info_vec, 16, true /* synch */, NULL);

    memset(malloc_table, 0, MAX_CHUNKS*sizeof(malloc_chunk_t));
}
