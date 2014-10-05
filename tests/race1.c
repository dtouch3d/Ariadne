#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 2

pthread_mutex_t run_lock;

int* array;

void *func1(void *threadid)
{
   long tid;
   tid = (long)threadid;

   array[0] = tid;

   printf("Hello World! It's me, thread #%ld!\n", tid);

   /*pthread_exit(NULL);*/
}


void *func2(void *threadid)
{
   long tid;
   tid = (long)threadid;

   array[0] = tid;

   printf("Hello World! It's me, thread #%ld!\n", tid);

   /*pthread_exit(NULL);*/
}

int main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;

   array = malloc(sizeof(int));

   rc = pthread_create(&threads[0], NULL, func1, (void *)1);
   rc = pthread_create(&threads[1], NULL, func2, (void *)2);

   sleep(2);

   free(array);
   /* Last thing that main() should do */
   pthread_exit(NULL);
}
