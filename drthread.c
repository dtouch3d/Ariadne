#include "dr_api.h"
#include "drwrap.h"
#include "drsyms.h"
#include "drmgr.h"
#include <string.h>

#include "drthread.h"

static void
event_module_load(void *drcontext, const module_data_t *info, bool loaded)
{
    int i,j;
    const char* modname = dr_module_preferred_name(info);
    module_handle_t modhandle = info->handle;

    /* We check if a library we want to instrument is loaded from
     * the global modtable array at drthread.h */
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
                void (*funcp)() = findfunc(func.name);
                /*dr_printf("[+] %s handler is at %p\n", func.name, funcp);*/

                /* Calls funcp before actual call */
                if(funcp != NULL)
                    drwrap_wrap((app_pc)addr, funcp, NULL);
            }
        }
    }
}

static void
event_thread_init(void* drcontext)
{
    thread_info_t* thread_info = dr_thread_alloc(drcontext, sizeof(*thread_info));

    drmgr_set_tls_field(drcontext, tls_index, thread_info);

    dr_mutex_lock(num_threads_lock);
    thread_info->tid = num_threads;
    num_threads++;
    dr_mutex_unlock(num_threads_lock);
}

static void
event_thread_exit(void* drcontext)
{
    thread_info_t* thread_info = (thread_info_t*)drmgr_get_tls_field(drcontext, tls_index);
    dr_thread_free(drcontext, thread_info, sizeof(*thread_info));
}

static void
event_exit(void)
{
    drwrap_exit();
    drmgr_exit();
    drsym_exit();
    umbra_exit();
    dr_mutex_destroy(num_threads_lock);
}

static dr_emit_flags_t
event_bb(void *drcontext, void *tag, instrlist_t *bb, bool for_trace, bool translating, OUT void **user_data)
{
}

DR_EXPORT void
dr_init(client_id_t id)
{
    drwrap_init();
    drmgr_init();
    drsym_init(0);
    umbra_init();

    drmgr_register_module_load_event(event_module_load);
    drmgr_register_thread_init_event(event_thread_init);
    drmgr_register_thread_exit_event(event_thread_exit);
    drmgr_register_bb_instrumentation_event(event_bb, NULL, NULL);

    dr_register_exit_event(event_exit);

    num_threads_lock = dr_mutex_create();
    tls_index = drmgr_register_tls_field();
}
