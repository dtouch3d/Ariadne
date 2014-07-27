typedef struct { char* name; void (*funcptr)(); } symtab_t;

static void
pthread_create_event(void *wrapcxt, OUT void **user_data);

static void
pthread_exit_event(void *wrapcxt, OUT void **user_data);

static void
pthread_mutex_lock_event(void *wrapcxt, OUT void **user_data);

static void
pthread_mutex_unlock_event(void *wrapcxt, OUT void **user_data);

/* Table mapping function names to functions.
 * http://c-faq.com/misc/symtab.html
 */

symtab_t pthread_symtab[] = {
    "libpthread",           NULL,
    "pthread_create",       pthread_create_event,
    "pthread_exit",         pthread_exit_event,
    "pthread_mutex_lock",   pthread_mutex_lock_event,
    "pthread_mutex_unlock", pthread_mutex_unlock_event
};

static void
pthread_create_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_create check here */
    dr_printf("[+] Hello from pthread_create_event\n");
    return;
}

static void
pthread_exit_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_exit check here */
    dr_printf("[+] Hello from pthread_exit_event\n");
    return;
}

static void
pthread_mutex_lock_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_mutex_lock check here */
    dr_printf("[+] Hello from pthread_mutex_lock_event\n");
    return;
}

static void
pthread_mutex_unlock_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_mutex_unlock check here */
    dr_printf("[+] Hello from pthread_mutex_unlock_event\n");
    return;
}
