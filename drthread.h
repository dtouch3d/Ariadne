#include "pthread_event.h"

#define SIZE(A) sizeof(A)/sizeof(A[0])

symtab_t* libtab[] = {pthread_symtab};

void (*findfunc(const char *name))()
{
    int i,j;

    for(i=0; i<SIZE(libtab); i++)
    {
        if(strcmp(name, libtab[i][0].name) == 0)
        {
            for(j=1; j<SIZE(libtab[i]); j++)
            {
                if(strcmp(name, libtab[i][0].name) == 0)
                    return libtab[i][j].funcptr;
            }
        }
    }
    return NULL;
}
