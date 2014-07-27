static void
pthread_create_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_create wrap here */
    dr_printf("[+] Hello from pthread_create_event\n");
    return;
}

static void
pthread_exit_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_exit wrap here */
    dr_printf("[+] Hello from pthread_exit_event\n");
    return;
}

static void
pthread_mutex_lock_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_mutex_lock wrap here */
    dr_printf("[+] Hello from pthread_mutex_lock_event\n");
    return;
}

static void
pthread_mutex_unlock_event(void *wrapcxt, OUT void **user_data)
{
    /* pthread_mutex_unlock wrap here */
    dr_printf("[+] Hello from pthread_mutex_unlock_event\n");
    return;
}
