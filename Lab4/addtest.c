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

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  *pointer = sum;
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
      add(values.counter,1);
      add(values.counter,-1);
    }
}

int main (int argc, char **argv) {
  int nIter;
  int nThreads;

  if (argc == 1) {
    nIter = 1;
    nThreads = 1;
  }
  else if (argc > 3) {
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

  //  pthread_t threads[nIter];
  struct arguments parameters;
  parameters.counter = &counter;
  parameters.iterations = nIter;
  pthread_t *threads = malloc(nThreads * sizeof(pthread_t*));
  /*
  struct timespec begin, end;
  clock_gettime(CLOCK_MONOTONIC, &begin);
  */
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

  //  pthread_exit(NULL);

  /*  while(nIter--) {
    add(&counter, 1);
    add(&counter, -1);
    }*/
  
  /*  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("DIFF: %d\n", end.tv_nsec - begin.tv_nsec);
  printf("WRONG: %d\n", counter);
  */
  fprintf(stdout, "%d\n", counter);
  
  return -1;
}
