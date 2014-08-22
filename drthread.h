#include "report.h"

#define SIZE(A) sizeof(A)/sizeof(A[0])

const char* const modtable[] =
{
    "libpthread",
    "libc"
};

static int num_threads = 0;
static int tls_index;

void* num_threads_lock;

typedef struct
{
    unsigned int tid;
} thread_info_t;

typedef struct
{
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

static void
malloc_event(void *wrapcxt, OUT void **user_data);

/* Table mapping function names to functions. Those
 * function must be defined in their respective header files.
 * http://c-faq.com/misc/symtab.html
 */

static symtab_t symtab[] = {
    /* pthread */
    "pthread_create",       pthread_create_event,
    "pthread_exit",         pthread_exit_event,
    "pthread_mutex_lock",   pthread_mutex_lock_event,
    "pthread_mutex_unlock", pthread_mutex_unlock_event,
    /* libc */
    "malloc" ,              malloc_event
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

static void
malloc_event(void *wrapcxt, OUT void **user_data)
{
    show_linenum(wrapcxt, __func__);
    return;
}
