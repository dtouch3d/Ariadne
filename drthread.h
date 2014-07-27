#include "pthread_event.h"

#define SIZE(A) sizeof(A)/sizeof(A[0])

typedef struct { char* name; void (*funcptr)(); } symtab_t;

const char* const threadlib[] = { "libpthread" };

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
