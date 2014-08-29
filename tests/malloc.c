#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	5

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;

   int* array;
   array = malloc(10*sizeof(int));

   printf("Hello World! It's me, thread #%ld!\n", tid);

   free(array);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
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

   /* Last thing that main() should do */
   pthread_exit(NULL);
}