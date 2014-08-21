#include "dr_api.h"
#include "drwrap.h"
#include "drsyms.h"
#include <string.h>

#include "drthread.h"

static void
event_module_load(void *drcontext, const module_data_t *info, bool loaded)
{
    int i,j;
    const char* modname = dr_module_preferred_name(info);
    module_handle_t modhandle = info->handle;

    /* We check if a library we want to instrument is loaded from
     * the global threadlib array at drthread.h */
    for(i=0; i<SIZE(threadlib); i++)
    {
        if(strstr(modname, threadlib[i]) != NULL)
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
event_exit(void)
{
    drwrap_exit();
    drsym_exit();
    dr_mutex_destroy(num_threads_lock);
}

DR_EXPORT void
dr_init(client_id_t id)
{
    drwrap_init();
    drsym_init(0);
    dr_register_module_load_event(event_module_load);
    dr_register_exit_event(event_exit);
    num_threads_lock = dr_mutex_create();
}
