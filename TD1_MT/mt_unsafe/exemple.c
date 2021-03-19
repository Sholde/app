#include <pthread.h>
#include <stdio.h>

#define COUNT_PER_THREAD 1000000
#define NUM_THREADS           10

volatile unsigned long count = 0;

void *parallel_count(void *args){
  int i;

  pthread_mutex_t *mut = (pthread_mutex_t *)args;
  
  for (i = 0; i < COUNT_PER_THREAD; ++i)
    {
      pthread_mutex_lock(mut);
      ++count;
      pthread_mutex_unlock(mut);
    }

  return NULL;
}

int main(int argc, char** argv)
{
  int i;
  pthread_t pid[NUM_THREADS];

  pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
   
  for (i = 0; i < NUM_THREADS; ++i)
    {
      pthread_create(&pid[i], NULL, parallel_count, &mut);
    }
   
  for (i = 0; i < NUM_THREADS; ++i)
    {
      pthread_join(pid[i], NULL);
    }

  pthread_mutex_destroy(&mut);
   
  printf("Valeur finale de count %ld \n", count);
   
  return 0;
}
