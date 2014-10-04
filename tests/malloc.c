#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	5

pthread_mutex_t run_lock;

int* array;

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;

   pthread_mutex_lock(&run_lock);

   array[tid] = tid;

   pthread_mutex_unlock(&run_lock);

   printf("Hello World! It's me, thread #%ld!\n", tid);

   /*pthread_exit(NULL);*/
}

int main(int argc, char *argv[])
{
   pthread_mutex_init(&run_lock, NULL);
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;

   array = malloc(NUM_THREADS*sizeof(int));

   for(t=0;t<NUM_THREADS;t++){
       rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
       if (rc){
           printf("ERROR; return code from pthread_create() is %d\n", rc);
           exit(-1);
       }
   }

   sleep(2);

   free(array);
   /* Last thing that main() should do */
   pthread_exit(NULL);
}
