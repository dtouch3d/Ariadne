#include "dr_api.h"

static void
event_thread_init(void *drcontext)
{
    dr_printf("[+] Created thread with tid #%d\n", dr_get_thread_id(drcontext));
}

static void
event_thread_exit(void *drcontext)
{
    dr_printf("[+] Thread exit event\n");
}

DR_EXPORT void
dr_init(client_id_t id)
{
    dr_register_thread_init_event(event_thread_init);
    dr_register_thread_exit_event(event_thread_exit);
}
