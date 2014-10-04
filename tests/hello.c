#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello from thread #%lu\n", tid);
   /*pthread_exit(NULL);*/
}

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    for(t=0;t<NUM_THREADS;t++){
        rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

   sleep(2);

   /* Last thing that main() should do */
   pthread_exit(NULL);
}
