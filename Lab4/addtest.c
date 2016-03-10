#include "addtest.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>

void usage(char **argv) {
  fprintf( stderr, "Usage: %s --iter=NINTERATIONS --threads=NTHREADS\n", argv[0] );
  exit(-1);
}

void (*add_function)(long long *pointer, long long value);

// this version of the add function does not use yield


void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  *pointer = sum;
}



/**************************************************************
/ QUESTION 1.1
*/

/* this version of the add function uses yield and as a result,
 * the function should be slower and cause more errors
 */


int opt_yield;


void add_yield(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    pthread_yield();

  *pointer = sum;
}


/**************************************************************
/ QUESTION 1.2
*/

/* these versions of the add function are for synchronization,
 * so the race conditions should be fixed when using these
 */

/* this version of the add function uses a pthread_mutex to sync
 * updates to the counter.
 */

pthread_mutex_t lock;

void add_mutex(long long *pointer, long long value) {
  pthread_mutex_lock(&lock);
  long long sum = *pointer + value;
  *pointer = sum;
  pthread_mutex_unlock(&lock);
}


/* this version of the add function uses the gcc atomic built-ins
 * to implement a spinlock and syncs updates that way
 */

volatile int spinlock;

void add_spinlock(long long *pointer, long long value) {
  while (__sync_lock_test_and_set(&spinlock, 1))
    {
      continue;
    }

  long long sum = *pointer + value;
  *pointer = sum;

  __sync_lock_release(&spinlock);    
}


/* this version of the add function uses an atomic operation to
 * update the counter instead of a locking mechanism like the other
 * two functions.
 */

void add_atomic(long long *pointer, long long value) {
  long long new;

  while(1)
    {
      new = *pointer + value;
      if (__sync_val_compare_and_swap(pointer, *pointer, new) + value
	  == new)
	break;
    }

  
}



struct arguments
{
  long long * counter;
  int iterations;
};

void *threadfunction(void *p)
{
  struct arguments values;
  values = *(struct arguments*) p;
  int iterations = values.iterations;

  int k;
  for (k = 0; k < iterations; k++)
    {
      add_function(values.counter,1);
      add_function(values.counter,-1);
    }
}

int main (int argc, char **argv) {
  int nIter;
  int nThreads;
  int status;
  status = 0;
  nIter = 1;
  nThreads = 1;

  add_function = add;
  
  if (argc > 4) {
    usage(argv);
  }

  int stop = 0;
  while(!stop) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    
    switch(c) {
    case 'i':
      nIter = atoi(optarg);
      if ( nIter < 1 ) {
	fprintf(stdout, "NITERATIONS must be > 1\n");
	usage(argv);
      }
      break;
    case 't':
      nThreads = atoi(optarg);
      if ( nThreads < 1 ) {
	fprintf(stdout, "NTHREADS must be > 1\n");
	usage(argv);
      }
      break;
    case 'y':
      opt_yield = atoi(optarg);

      if (opt_yield != 1)
	{
	  fprintf(stdout, "yield should be 1 or not used, yield set to 0\n");
	  opt_yield = 0;
	}

      add_function = add_yield;
      break;
    case 's':
      switch(*optarg)
	{
	case 'm':
	  add_function = add_mutex;
	  break;
	case 's':
	  add_function = add_spinlock;
	  break;
	case 'c':
	  add_function = add_atomic;
	  fprintf(stdout,"here\n");
	  break;
	default:
	  fprintf(stderr, "ERROR: sync option %s not recognized\n", optarg);
	  exit(1);
	}
      
      break;
    case -1: // finished
      ++stop;
      break;
      
    default:
      usage(argv);
      ++stop;
      break;
    }
  }

  fprintf( stdout,
	   "%d threads x %d iterations x (add + subtract) = %d operations\n",
	   nThreads, nIter, 2 * nThreads * nIter);

  long long counter = 0;

  pthread_mutex_init(&lock, NULL);
  spinlock = 0;
  
  //  pthread_t threads[nIter];
  struct arguments parameters;
  parameters.counter = &counter;
  parameters.iterations = nIter;
  pthread_t *threads = malloc(nThreads * sizeof(pthread_t*));
  
  struct timespec begin, end;
  clock_gettime(CLOCK_MONOTONIC, &begin);
  
  int t;
  int err;
  for(t=0; t<nThreads; t++) {
    err = pthread_create(&threads[t], NULL, threadfunction, &parameters);
    if(err) {
      fprintf(stdout, "ERROR; pthread_create() failed with %d\n", err);
      exit(-1);
    }
  }

  for(t=0; t<nThreads; t++) {
    pthread_join(threads[t],NULL);
  }

  free(threads);

  if (counter != 0)
    {
      fprintf(stderr,"ERROR: final count = %d\n",counter);
      status = 1;
    }

  clock_gettime(CLOCK_MONOTONIC, &end);

  long time_elapsed = (end.tv_nsec - begin.tv_nsec) + (end.tv_sec - begin.tv_sec) * 1e9;

  fprintf(stdout, "elapsed time: %d ns\n", time_elapsed);
  fprintf(stdout, "per operation: %d ns\n", time_elapsed / (2 * nThreads * nIter));
  
  //  pthread_exit(NULL);

  /*  while(nIter--) {
    add(&counter, 1);
    add(&counter, -1);
    }*/
  
  /*  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("DIFF: %d\n", end.tv_nsec - begin.tv_nsec);
  printf("WRONG: %d\n", counter);
  */


  pthread_mutex_destroy(&lock);
  exit(status);
}
