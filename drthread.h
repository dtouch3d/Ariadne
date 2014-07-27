#include "pthread_event.h"

#define SIZE(A) sizeof(A)/sizeof(A[0])

symtab_t libtab[] = {pthread_symtab};

void (*findfunc(const char *name))()
{
    int i;

    for(i = 0; i < SIZE(pthread_symtab); i++) {
        if(strcmp(name, pthread_symtab[i].name) == 0)
            return pthread_symtab[i].funcptr;
        }

    return NULL;
}
