#include "dr_api.h"

static void
event_thread_init(void *drcontext)
{
    static unsigned int tid = 0;
    tid++;
    dr_printf("[+] Created thread #%d\n", tid);
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
