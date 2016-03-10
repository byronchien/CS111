#include "SortedList.h"
#include "sltest.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void usage(char **argv) {
  fprintf( stderr, "Usage: %s --threads=NTHREADS --iter=NINTERATIONS --yield=[ids]\n", argv[0] );
  exit(-1);
}

int main (int argc, char **argv) {
  int nIter;
  int nThreads;
  char yields[3];
  int q;
  for(q = 0; q < 3; q++)
    yields[q] = 0;

  nIter = 1;
  nThreads = 1;
  
  if (argc > 4)
    usage(argv);

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
      strncpy(yields, optarg, 3); // only 3 possible yields
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

  // setup for opt_yield bitmask
  opt_yield = 0;
  int i;
  for (i=0; i<3; i++) {
    switch(yields[i]) {
    case 'i':
      opt_yield |= 0x01;
      break;
    case 'd':
      opt_yield |= 0x02;
      break;
    case 's':
      opt_yield |= 0x04;
      break;
    case 0:
      break;
    default:
      fprintf(stdout, "Yield value '%c' not recognized.\n", yields[i]);
      usage(argv);
    }
  }
  
  SortedList_t *list = malloc(sizeof(SortedList_t));
  list->next = list;
  list->prev = list;
  list->key = NULL;
  
  int nElements = nThreads * nIter;
  printf("nElements: %d\n", nElements);
  SortedListElement_t * elements[nElements];
  
  srand(time(NULL));
  for(i=0; i<nElements; i++ ) {
    elements[i] = malloc(sizeof(SortedListElement_t));
    
    char randkey[9];
    int j = 0;
    while (j < 8) { // generate random key
      int random = rand() % 123; // 122 is highest ASCII int for alpha
      if(isalpha(random)) {
	randkey[j] = (char) random;
	j++;
      }
    }
    randkey[8] = 0;
    printf("KEY%d: %s\n", i, randkey);
    elements[i]->key = &randkey[0];
  }

  struct timespec begin, end;
  clock_gettime(CLOCK_MONOTONIC, &begin);
  ////////////////////////////////////////
  //////BYRONE PUT ALL YO CRAP HERE///////
  ////////////////////////////////////////
  clock_gettime(CLOCK_MONOTONIC, &end);

  if (SortedList_length(list) != 0) {
    fprintf(stderr, "Final length of list is not 0\n");
  }

  long nOps = nThreads*nIter*2*nElements;
  long timediff = end.tv_nsec - begin.tv_nsec;
  fprintf(stdout,
	  "%d threads x %d iterations x (ins + lookup/del) x (%d/2 avg len) = %d operations\n",
	  nThreads, nIter, nElements, nOps);
  fprintf(stdout, "elapsed time: %d ns\n", timediff);
  fprintf(stdout, "per operation: %d ns\n", timediff/nOps);

  free(list);
  for(i=0; i<nElements; i++)
    free(elements[i]);
  
  return 0;
}
