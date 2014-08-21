#include "report.h"

#define SIZE(A) sizeof(A)/sizeof(A[0])

const char* const threadlib[] = { "libpthread" };

static num_threads = 0;

void* num_threads_lock;

typedef struct {
    unsigned int tid;
} thread_info_t;

typedef struct {
    char* name;
    void (*funcptr)();
} symtab_t;


static void
pthread_create_event(void *wrapcxt, OUT void **user_data);

static void
pthread_exit_event(void *wrapcxt, OUT void **user_data);

static void
pthread_mutex_lock_event(void *wrapcxt, OUT void **user_data);

static void
pthread_mutex_unlock_event(void *wrapcxt, OUT void **user_data);

/* Table mapping function names to functions. Those
 * function must be defined in their respective header files.
 * http://c-faq.com/misc/symtab.html
 */

static symtab_t symtab[] = {
    /* pthread */
    "pthread_create",       pthread_create_event,
    "pthread_exit",         pthread_exit_event,
    "pthread_mutex_lock",   pthread_mutex_lock_event,
    "pthread_mutex_unlock", pthread_mutex_unlock_event
};

/* XXX: Optimize ? */
void (*findfunc(const char *name))()
{
    int i;

    for(i=0; i<SIZE(symtab); i++)
    {
        if(strcmp(name, symtab[i].name) == 0)
            return symtab[i].funcptr;
    }
    return NULL;
}

static void
pthread_create_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_create wrap here */
    show_linenum(wrapcxt, __func__);

    app_pc drcontext = drwrap_get_drcontext(wrapcxt);

    thread_info_t* thread_info = dr_thread_alloc(drcontext, sizeof(thread_info_t));

    dr_set_tls_field(drcontext, thread_info);

    dr_mutex_lock(num_threads_lock);
    thread_info->tid = num_threads;
    num_threads++;
    dr_mutex_unlock(num_threads_lock);


    dr_printf("[+] Thread #%d created!\n", thread_info->tid);

    return;
}

static void
pthread_exit_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_exit wrap here */
    show_linenum(wrapcxt, __func__);
    return;
}

static void
pthread_mutex_lock_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_mutex_lock wrap here */
    show_linenum(wrapcxt, __func__);
    return;
}

static void
pthread_mutex_unlock_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_mutex_unlock wrap here */
    show_linenum(wrapcxt, __func__);
    return;
}
