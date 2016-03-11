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
#include <math.h>

void usage(char **argv) {
  fprintf( stderr, "Usage: %s --threads=NTHREADS --iter=NINTERATIONS --yield=[ids] --sync=[ms] --lists=NLISTS\n", argv[0] );
  exit(-1);
}

pthread_mutex_t lock;
volatile int spinlock;

struct arguments
{
  SortedList_t *list;
  SortedListElement_t ** elements;
  int nIter,start;
};

void (*insert) (SortedList_t *list, SortedListElement_t *element);
int (*delete) (SortedListElement_t *element);
SortedListElement_t* (*lookup) (SortedList_t *list, const char *key);
int (*list_length) (SortedList_t *list);

void *threadfunction(void*p)
{

  struct arguments values;
  values = *(struct arguments*) p;

  int k;
  for (k = 0; k < values.nIter; k++)
    {
      insert(values.list, values.elements[k]);
    }

  
  int length;
  length = list_length(values.list);

  if (length == -1)
    {
      fprintf(stdout, "ERROR: SortedList_length returned %d\n", length);
      exit(1);
    }

  fprintf(stdout, "List length: %d\n", length);

  SortedListElement_t* query;

  int status;
  status = 0;
  
  for (k = 0; k < values.nIter; k++)
    {
      query = lookup(values.list, values.elements[k]->key);
      status = delete(query);

      if (status != 0)
	{
	  fprintf(stdout, "ERROR: SortedList_delete returned &d\n",status);
	  exit(1);
	}
    }

}


int chooselist(char * key, int nLists)
{
  int k = 111;
  long hash = 0;
  
  int i = 0;
  while(key[i] != 0) {
    hash += key[i] * exp(k, i);
  }

  return hash % nLists;
}


int main (int argc, char **argv) {
  int nIter;
  int nThreads;
  char yields[3];
  int q;
  for (q=0; q<3; q++)
    yields[q] = 0;
  pthread_mutex_init(&lock, NULL);
  spinlock = 0;
  int nLists;

  insert = SortedList_insert;
  delete = SortedList_delete;
  lookup = SortedList_lookup;
  list_length = SortedList_length;
  
  nIter = 1;
  nThreads = 1;
  nLists = 1;
  
  if (argc > 6)
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
    case 's':
      switch(*optarg)
	{
	case 's':
	  insert = SortedList_insert_s;
	  delete = SortedList_delete_s;
	  lookup = SortedList_lookup_s;
	  list_length = SortedList_length_s;
	  break;
	case 'm':
	  insert = SortedList_insert_m;
	  delete = SortedList_delete_m;
	  lookup = SortedList_lookup_m;
	  list_length = SortedList_length_m;
	  break;
	default:
	  fprintf(stderr, "ERROR: sync option &s not recognized\n", optarg);
	  exit(1);
	}
      break;

    case 'l':
      nLists = atoi(optarg);
      if ( nLists < 1 ) {
	fprintf(stdout, "NLISTS must be > 1\n");
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

  SortedList_t * listlist = malloc(sizeof(SortedList_t*) * nLists);
  for(i=0; i<nLists; i++) {
    listlist[i] = malloc(sizeof(SortedList_t));
    listlist[i]->next = list;
    listlist[i]->prev = list;
    listlist[i]->key = NULL;
  }
  
  int nElements = nThreads * nIter;
  SortedListElement_t** elements = malloc(sizeof(SortedListElement_t*) * nElements);
  char** keys = malloc(sizeof(char*) * nElements);

  /* Generate Keys */
  srand(time(NULL));
  for(i=0; i<nElements; i++ ) {
    elements[i] = malloc(sizeof(SortedListElement_t));
    keys[i] = malloc(sizeof(char) * 9);
    int j = 0;
    while (j < 8) { // generate random key
      int random = rand() % 123; // 122 is highest ASCII int for alpha
      if(isalpha(random)) {
	keys[i][j] = (char) random;
	j++;
      }
    }
    keys[i][8] = 0;
    printf("KEY%d: %s\n", i, keys[i]);
    elements[i]->key = keys[i];
  }

  pthread_t * threads = malloc(nThreads * sizeof(pthread_t*));

  struct arguments* parameters;
  parameters = malloc(nThreads * sizeof(struct arguments));
  
  struct timespec begin, end;
  clock_gettime(CLOCK_MONOTONIC, &begin);

  
  int t;
  int err;
  for(t = 0; t < nThreads; t++) {
    parameters[t].list = list;
    parameters[t].nIter= nIter;
    parameters[t].elements = &elements[t*nIter];
    //    parameters.elements = elements + t*nIter;
    err = pthread_create(&threads[t], NULL, threadfunction, &parameters[t]);

    if(err) {
      fprintf(stdout, "ERROR; pthread_create() failed with &d\n", err);
      exit(-1);
    }
  }

  for(t = 0; t < nThreads; t++) {
    pthread_join(threads[t],NULL);
  }


  
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
  
  free(threads);
  free(parameters);
  pthread_mutex_destroy(&lock);
  for(i=0; i<nElements; i++) {
    free(elements[i]);
    free(keys[i]);
    free(listlist[i]);
  }
  free(elements);
  free(keys);
  free(listlist);
  
  return 0;
}
