#include "dr_api.h"

static void
event_thread_init(void *drcontext)
{
    dr_printf("[+] Thread #%d created\n", dr_get_thread_id(drcontext));
}

static void
event_thread_exit(void *drcontext)
{
    dr_printf("[+] Thread #%d exit\n", dr_get_thread_id(drcontext));
}


static void
event_module_load(void *drcontext, const module_data_t *info, bool loaded)
{
    dr_printf("[+] Loaded module %s\n", dr_module_preferred_name(info));
}

DR_EXPORT void
dr_init(client_id_t id)
{
    dr_register_thread_init_event(event_thread_init);
    dr_register_thread_exit_event(event_thread_exit);
    dr_register_module_load_event(event_module_load);
}
