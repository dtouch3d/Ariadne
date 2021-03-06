#include "drvector.h"

#define SIZE(A) sizeof(A)/sizeof(A[0])

/* XXX: For now a maximum of 8 locks, the size of bits in the shadow lock set memory
 * A bit is set if the corresponding lock was held during the memory access */
#define MAX_LOCKS 8

#define MAX_STR 100

static umbra_map_t* umbra_map;

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
    void* addr;
    byte nonlocker;
    bool alive;
} lock_info_t;

typedef struct
{
    unsigned int tid;
    byte lockset;
    size_t num_locks_held;
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


/* Holds a thread_info struct of each thread */
drvector_t* thread_info_vec;

static drvector_t* malloc_table;
static int num_malloc_chunk;

/* contains the info of every lock. Each thread has a bitmask representing which
 * locks are held during execution */

lock_info_t lock[MAX_LOCKS];

static size_t num_locks = 0;
void* lock_mutex;

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
    int i;
    drvector_lock(malloc_table);
    for (i=0; i<malloc_table->entries; i++)
    {
        malloc_chunk_t* chunk = drvector_get_entry(malloc_table, i);
        if (addr >= chunk->addr && addr < chunk->addr + chunk->size)
        {
            dr_printf("[+] in malloc chunk at %p, size %d\n",
                      chunk->addr, chunk->size);
            drvector_unlock(malloc_table);
            return true;
        }
    }
    drvector_unlock(malloc_table);
    return false;
}

bool
in_main_module(app_pc addr)
{
    //return true;
    module_data_t* main_module = dr_get_main_module();

    return dr_module_contains_addr(main_module, addr);
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
    show_linenum(wrapcxt, __func__);

    thread_info_t* thread_info = get_thread_info(wrapcxt);

    /* pthread_exit crashes our program and holds 10+ locks
     * skipping below crashes anyways
     */
    if (thread_info->tid > 0)
    {
        dr_printf("[+] thread #%d exiting\n", thread_info->tid);
        drwrap_skip_call(wrapcxt, NULL, 1);
    }
    return;
}

static void
pthread_mutex_lock_event(void *wrapcxt, void **user_data)
{
    /* pthread_mutex_lock wrap here */
    void* lock_arg = drwrap_get_arg(wrapcxt, 0);

    thread_info_t* thread_info = get_thread_info(wrapcxt);

    /* in "main" */
    if (thread_info->tid == 0)
        return;


    dr_mutex_lock(lock_mutex);

    //dr_printf("num_locks: %d\n", num_locks);

    int i;
    for (i=0; i<num_locks; i++)
    {
        if (lock[i].addr == lock_arg)
        {
            dr_printf("[+] thread # %d lock exists @ %p\n", thread_info->tid, lock_arg);
            //lock[i].alive = 1;

            thread_info->lockset |= 1 << i;
            thread_info->num_locks_held++;

            dr_mutex_unlock(lock_mutex);

            return;
        }
    }

    lock[num_locks].addr = lock_arg;
    //lock[num_locks].alive = 1;

    num_locks++;

    dr_mutex_unlock(lock_mutex);

    if (num_locks >= MAX_LOCKS)
    {
        dr_printf("[!!] max thread locks exceeded\n");
    }

    thread_info->lockset |= 1 << i;
    thread_info->num_locks_held++;

    show_linenum(wrapcxt, __func__);
    return;
}

static void
pthread_mutex_unlock_event(void *wrapcxt, void **user_data)
{
    void* lock_arg = drwrap_get_arg(wrapcxt, 0);

    thread_info_t* thread_info = get_thread_info(wrapcxt);

    /* in "main" */
    if (thread_info->tid == 0)
        return;

    dr_mutex_lock(lock_mutex);
    int i;
    for (i=0; i<num_locks; i++)
    {
        if (lock[i].addr == lock_arg)
        {
            dr_printf("[+] thread #%d killed lock @ %p\n", thread_info->tid, lock_arg);
            //lock[i].alive = 0;

            thread_info->lockset &= (0 << i);

            dr_mutex_unlock(lock_mutex);
            return;
        } else {
            dr_printf("[+] thread #%d lock @ %p\n != lock[%d] @ %p", thread_info->tid, lock_arg, i, lock[i].addr);
        }

    }

    dr_mutex_unlock(lock_mutex);

    dr_printf("[!!] held lock not found on global lock array!\n");

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

    malloc_chunk_t* chunk = dr_global_alloc(sizeof(malloc_chunk_t));
    chunk->addr = retval;
    chunk->size = (size_t)user_data;

    drvector_lock(malloc_table);

    drvector_append(malloc_table, chunk);

    drvector_unlock(malloc_table);

    /* create shadow memory */
    if (umbra_create_shadow_memory(umbra_map, 0 /*options*/, retval, (size_t)user_data,
                                   0 /* default value */, 1 /*value size*/) != DRMF_SUCCESS)
    {
        dr_printf("[!] shadow memory create error for memory %p\n", retval);
    }

    dr_printf("[+] Malloc : addr = %p size = %d\n", retval, (size_t)user_data);
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

    pbag->entries = 0;

    return;
}

static thread_info_t*
get_thread_info_helper(void* wrapcxt, bool is_drcontext)
{
    void* context;
    if (!is_drcontext)
    {
        context = drwrap_get_drcontext(wrapcxt);
    } else
    {
        context = wrapcxt;
    }


    unsigned int* tid = drmgr_get_tls_field(context, tls_index);
    return drvector_get_entry(thread_info_vec, *tid);
}

thread_info_t*
get_thread_info(void* wrapcxt)
{
    return get_thread_info_helper(wrapcxt, false);
}

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
    dr_printf("[+] %s at %s:%d\n", funcname, sym.file, sym.line);
}

static void
print_bag(drvector_t* bag)
{
    int i;
    dr_printf("[+] entries: %d, [", bag->entries);
    for(i=0; i<bag->entries; i++)
    {
        unsigned int stid = drvector_get_entry(bag, i);
        dr_printf("%d ", stid);
    }
    dr_printf("]\n");
}

bool
in_drvector(drvector_t *vec, void* elem)
{
    int i;
    for (i=0; i<vec->entries; i++)
    {
        if (drvector_get_entry(vec, i) == elem)
        {
            return true;
        }
    }
    return false;
}

/****  Shadow Memory ****/

/* We use 2 byte per 1 bytes of memory, indicating the thread of the last
 * access to memory and the set of locks held during mem ref as bitmask to the
 * thread's lock array.
 */
#define SHADOW_GRANULARITY 1
#define SHADOW_MAP_SCALE   UMBRA_MAP_SCALE_UP_2X

#define SHADOW_DEFAULT_VALUE      19
#define SHADOW_DEFAULT_VALUE_SIZE 1


void
shadow_get_byte(app_pc addr, byte* shadow_bytes)
{
    /*byte* val = dr_global_alloc(2);*/
    size_t app_size = SHADOW_GRANULARITY;
    size_t shdw_size = 2;
    int res =umbra_read_shadow_memory(umbra_map, addr, app_size, &shdw_size, shadow_bytes);

    if (res != DRMF_SUCCESS || shdw_size != 2)
    {
        dr_printf("[!] failed to get shadow byte of %p : %d\n", addr, res);
    }
    /*dr_printf("[!] (get) shdw_size: %d, sizeof(val): %d\n", shdw_size, 2);*/
    /*return val;*/
}

void
shadow_set_byte(app_pc addr, byte* val)
{
    size_t app_size = SHADOW_GRANULARITY;
    size_t shdw_size = 2;
    int res = umbra_write_shadow_memory(umbra_map, addr, app_size, &shdw_size, val);
    if (res != DRMF_SUCCESS || shdw_size != 2)
    {
        dr_printf("[!] failed to set shadow byte of %p : %d\n", addr, res);
    }
    /*dr_printf("[!] (set) shdw_size: %d, sizeof(val): %d\n", shdw_size, 2);*/
}


static void
shadow_memory_init(void)
{
    umbra_map_options_t umbra_map_ops;

    memset(&umbra_map_ops, 0, sizeof(umbra_map_ops));
    umbra_map_ops.struct_size = sizeof(umbra_map_ops);
    umbra_map_ops.flags = UMBRA_MAP_CREATE_SHADOW_ON_TOUCH;
    umbra_map_ops.scale = SHADOW_MAP_SCALE;
    umbra_map_ops.default_value = SHADOW_DEFAULT_VALUE;
    umbra_map_ops.default_value_size = SHADOW_DEFAULT_VALUE_SIZE;

    if (umbra_create_mapping(&umbra_map_ops, &umbra_map) != DRMF_SUCCESS)
        dr_printf("[!] failed to create shadow memory mapping");
}

void
shadow_memory_destroy(void)
{
    if (umbra_destroy_mapping(umbra_map) != DRMF_SUCCESS)
        dr_printf("[!] failed to destroy shadow memory");
}


