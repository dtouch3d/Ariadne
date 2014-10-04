#include "drvector.h"

#define SIZE(A) sizeof(A)/sizeof(A[0])

/* XXX: For now a maximum of 8 locks, the size of bits in the shadow lock set memory
 * A bit is set if the corresponding lock was held during the memory access */
#define MAX_LOCKS 8
#define MAX_CHUNKS 1000
#define MAX_STR 100

umbra_map_t *umbra_map;

static char* const modtable[] =
{
    "libpthread",
    "libc"
};

static pthread_mutex_t run_lock;
static int num_threads = 0;
static void* num_threads_lock;

static int tls_index;

typedef struct
{
    uintptr_t tid;
    void* lock[MAX_LOCKS];
    size_t num_locks;
    drvector_t* sbag;
    drvector_t* pbag;
} thread_info_t;

typedef struct
{
    char* name;
    void (*pre_func_cb)();
    void (*post_func_cb)();
} symtab_t;

typedef struct
{
    app_pc addr;
    size_t size;
} malloc_chunk_t;


drvector_t* main_sbag;
drvector_t* main_pbag;

/* Holds a thread_info struct of each thread */
drvector_t* thread_info_vector;

static malloc_chunk_t malloc_table[MAX_CHUNKS];
static int num_malloc_chunk;
static void* malloc_table_lock;

static void
pthread_create_event(void *wrapcxt, void **user_data);

static void
pthread_join_event(void *wrapcxt, void **user_data);

static void
pthread_exit_event(void *wrapcxt, void **user_data);

static void
pthread_mutex_lock_event(void *wrapcxt, void **user_data);

static void
pthread_mutex_unlock_event(void *wrapcxt, void **user_data);

static void
malloc_pre_event(void *wrapcxt, void **user_data);

static void
malloc_post_event(void *wrapcxt, void *user_data);

static thread_info_t*
get_thread_info(void* wrapcxt);

static void
show_linenum(void* wrapcxt, const char* funcname);

static bool
in_malloc_chunk(app_pc addr)
{
    dr_mutex_lock(malloc_table_lock);
    int i;
    for (i=0; i<num_malloc_chunk; i++)
    {
        malloc_chunk_t chunk = malloc_table[i];
        if (addr >= chunk.addr && addr < chunk.addr + chunk.size)
        {
            dr_mutex_unlock(malloc_table_lock);
            dr_printf("in malloc chunk at %p, size %d\n", chunk.addr, chunk.size);
            return true;
        }
    }
    dr_mutex_unlock(malloc_table_lock);
    return false;
}

bool
in_main_module(app_pc addr)
{
    //return true;
    module_data_t* main_module = dr_get_main_module();

    return dr_module_contains_addr(main_module, addr);
}

void
print_malloc_chunks(void)
{
    dr_mutex_lock(malloc_table_lock);
    int i;
    for (i=0; i<num_malloc_chunk; i++)
    {
        malloc_chunk_t chunk = malloc_table[i];
        dr_printf("[+] malloc chunk #%d at %p, size %d\n", i, chunk.addr, chunk.size);
    }
    dr_mutex_unlock(malloc_table_lock);
}

/* Table mapping function names to functions. Those
 * function must be defined in their respective header files.
 * http://c-faq.com/misc/symtab.html
 */

static symtab_t symtab[] = {
    /* pthread */
    "pthread_create",       pthread_create_event, NULL,
    "pthread_exit",         pthread_exit_event, NULL,
    "pthread_mutex_lock",   pthread_mutex_lock_event, NULL,
    "pthread_mutex_unlock", pthread_mutex_unlock_event, NULL,
    "pthread_join",         pthread_join_event, NULL,
    /* libc */
    "malloc" ,              malloc_pre_event, malloc_post_event
};

/* XXX: Optimize ? */
static int
findfunc(const char *name)
{
    int i;

    for(i=0; i<SIZE(symtab); i++)
    {
        if(strcmp(name, symtab[i].name) == 0)
            return i;
    }
    return -1;
}

static void
thread_func_pre(void *wrapcxt, void **user_data)
{
    //dr_printf("thread_func_pre\n");
    //pthread_mutex_lock(&run_lock);
    return;
}

static void
pthread_create_event(void *wrapcxt, void **user_data)
{
    /* pthread_create wrap here */
    show_linenum(wrapcxt, __func__);

    void* (*thread_func)(void*) = (void* (*)(void*))drwrap_get_arg(wrapcxt, 2);

    /* thread_func_post would never be called because of pthread_exit */
    drwrap_wrap((app_pc)thread_func, thread_func_pre, NULL);

    return;
}

static void
pthread_exit_event(void *wrapcxt, void **user_data)
{
    /* pthread_exit wrap here */
    //show_linenum(wrapcxt, __func__);

    thread_info_t* thread_info = get_thread_info(wrapcxt);

    if (thread_info->tid > 0)
    {
        dr_printf("thread #%d exiting\n", thread_info->tid);
        drwrap_skip_call(wrapcxt, NULL, 1);
    }
    return;
}

static void
pthread_mutex_lock_event(void *wrapcxt, void **user_data)
{
    /* pthread_mutex_lock wrap here */
    void* lock = drwrap_get_arg(wrapcxt, 0);

    thread_info_t* thread_info = get_thread_info(wrapcxt);

    /* in "main" */
    if (thread_info->tid == 0)
        return;

    thread_info->lock[thread_info->num_locks] = lock;
    thread_info->num_locks++;

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
malloc_pre_event(void *wrapcxt, void **user_data)
{
    thread_info_t* thread_info = get_thread_info(wrapcxt);

    /* not in "main" */
    if (thread_info->tid != 0)
        return;

    size_t size = (size_t)drwrap_get_arg(wrapcxt, 0);
    *user_data = (void*) size;

    show_linenum(wrapcxt, __func__);
    return;
}

static void
malloc_post_event(void *wrapcxt, void *user_data)
{
    thread_info_t* thread_info = get_thread_info(wrapcxt);

    /* if not in "main" or not in main executable return */
    if (thread_info->tid != 0 || !in_main_module(drwrap_get_retaddr(wrapcxt)-sizeof(void*)))
        return;

    app_pc retval = drwrap_get_retval(wrapcxt);

    dr_mutex_lock(malloc_table_lock);
    int idx = num_malloc_chunk;
    num_malloc_chunk++;
    dr_mutex_unlock(malloc_table_lock);

    malloc_table[idx].addr = retval;
    malloc_table[idx].size = (size_t)user_data;

    /* create shadow memory */
    if (umbra_create_shadow_memory(umbra_map, 0 /*options*/, retval, (size_t)user_data,
                                   0 /* default value */, 1 /*value size*/) != DRMF_SUCCESS)
    {
        dr_printf("[!] shadow memory create error for memory %p\n", retval);
    }

    dr_printf("Malloc : addr = %p size = %d\n", retval, (size_t)user_data);
}

static void
pthread_join_event(void *wrapcxt, void **user_data)
{
    thread_info_t* thread_info = get_thread_info(wrapcxt);

    int i;

    drvector_t* sbag = thread_info->sbag;
    drvector_t* pbag = thread_info->pbag;

    for(i=0; i<thread_info->pbag->entries; i++)
    {
        void* data = drvector_get_entry(pbag, i);
        drvector_append(sbag, data);
    }

    //drvector_delete(pbag);
    //drvector_init(pbag, 16, true [>synch<], NULL);

    pbag->entries = 0;

    return;
}

static thread_info_t*
get_thread_info(void* wrapcxt)
{
    void* drcontext = drwrap_get_drcontext(wrapcxt);
    thread_info_t* thread_info = (thread_info_t*)drmgr_get_tls_field(drcontext, tls_index);
    return thread_info;
}

/* funcname is evaluated at compile time with __func__ macro by the caller */
static void
show_linenum(void* wrapcxt, const char* funcname)
{
    return;
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

static void
print_bag(drvector_t* bag)
{
    int i;
    for(i=0; i<bag->entries; i++)
    {
        uintptr_t stid = drvector_get_entry(bag, i);
        dr_printf("%d ", stid);
    }
    dr_printf("\n");
}
