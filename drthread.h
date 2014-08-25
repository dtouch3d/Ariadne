#define SIZE(A) sizeof(A)/sizeof(A[0])
#define MAX_LOCKS 10
#define MAX_CHUNKS 100

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

typedef struct
{
    void* addr;
    size_t size;
} malloc_chunk_t;


static malloc_chunk_t malloc_chunck_table[MAX_CHUNKS];
static int num_malloc_chunk;
void* malloc_chunk_lock;

static void
pthread_create_event(void *wrapcxt, void **user_data);

static void
pthread_exit_event(void *wrapcxt, void **user_data);

static void
pthread_mutex_lock_event(void *wrapcxt, void **user_data);

static void
pthread_mutex_unlock_event(void *wrapcxt, void **user_data);

static void
malloc_event(void *wrapcxt, void **user_data);

static void
show_linenum(void* wrapcxt, const char* funcname);

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
pthread_create_event(void *wrapcxt, void **user_data)
{
    /* pthread_create wrap here */
    show_linenum(wrapcxt, __func__);
    return;
}

static void
pthread_exit_event(void *wrapcxt, void **user_data)
{
    /* pthread_exit wrap here */
    show_linenum(wrapcxt, __func__);
    return;
}

static void
pthread_mutex_lock_event(void *wrapcxt, void **user_data)
{
    /* pthread_mutex_lock wrap here */
    show_linenum(wrapcxt, __func__);
    return;
}

static void
pthread_mutex_unlock_event(void *wrapcxt, void **user_data)
{
    /* pthread_mutex_unlock wrap here */
    show_linenum(wrapcxt, __func__);
    return;
}

static void
malloc_event(void *wrapcxt, void **user_data)
{
    /* TODO: Save arg on user_data and pass to post_cb and save there.
     *       Also check if malloc fails!
     */
    dr_mutex_lock(malloc_chunk_lock);
    //malloc_chunk_table[num_malloc_chunk].size = drwrap_get_arg(0);
    num_malloc_chunk++;
    dr_mutex_unlock(malloc_chunk_lock);
    show_linenum(wrapcxt, __func__);
    return;
}


#define MAX_STR 100

/* funcname is evaluated at compile time with __func__ macro by the caller */
static void
show_linenum(void* wrapcxt, const char* funcname)
{
    /* Need to substract to find the call address */
    app_pc addr = drwrap_get_retaddr(wrapcxt)-sizeof(void*);
    module_data_t* modinfo = dr_lookup_module(addr);

    if (modinfo == NULL)
        return;

    /* Naive approach to ignore calls from other modules */
    const char* appname = dr_get_application_name();
    if (strstr(modinfo->full_path, appname) == NULL)
        return;

    drsym_info_t sym;
    char name[MAX_STR];
    char file[MAX_STR];
    sym.struct_size = sizeof(sym);
    sym.name = name;

    sym.name_size = sizeof(name);
    sym.file_size = sizeof(file);
    sym.file = file;

    drsym_lookup_address(modinfo->full_path, addr-modinfo->start, &sym, DRSYM_DEFAULT_FLAGS);

    /* Should probably check for the existence of symbols
     * and print hex address otherwise
     */
    dr_printf("%s at %s:%d\n", funcname, sym.file, sym.line);
}
