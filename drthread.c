#include "dr_api.h"
#include <string.h>

#define SIZE(A) sizeof(A)/sizeof(A[0])

static int
pthread_create_event(void *wrapcxt, OUT void **user_data);

/* Table mapping function names to functions.
 * http://c-faq.com/misc/symtab.html
 */
struct { char* name; int (*funcptr)(); } pthread_symtab[] = {
    "pthread_create", pthread_create_event,
};

static int
pthread_create_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_create check here */
    dr_printf("[+] Hello from pthread_create_event\n");
    return;
}

int (*findfunc(const char *name))()
{
    int i;

    for(i = 0; i < SIZE(pthread_symtab); i++) {
        if(strcmp(name, pthread_symtab[i].name) == 0)
            return pthread_symtab[i].funcptr;
        }

    return NULL;
}


const char* const threadlib[] = { "libpthread" };

const char* const threadfuncs[][10] = { {"pthread_create"} };

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
    int i,j;
    const char* lib = dr_module_preferred_name(info);
    module_handle_t lib_handle = info->handle;

    /* We check if a library we want to instrument is loaded */
    for(i=0; i<SIZE(threadlib); i++)
    {
        if(strstr(lib, threadlib[i]) != NULL)
        {
            dr_printf("[+] Loaded %s library\n", lib);
            for(j=0; j<SIZE(threadfuncs[i]) && threadfuncs[i][j] != NULL; j++)
            {
                void* addr = dr_get_proc_address(lib_handle, threadfuncs[i][j]);
                dr_printf("[+] Found %s @ %p\n", threadfuncs[i][j], addr);
                int (*funcp)() = findfunc(threadfuncs[i][j]);
                dr_printf("%s handler is at %p\n", threadfuncs[i][j], funcp);
                if(funcp != NULL)
                    (*funcp)();
            }
        }
    }
}

DR_EXPORT void
dr_init(client_id_t id)
{
    dr_register_thread_init_event(event_thread_init);
    dr_register_thread_exit_event(event_thread_exit);
    dr_register_module_load_event(event_module_load);
}
