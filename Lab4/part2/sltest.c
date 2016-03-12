#include "SortedList.h"
#include "sltest.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void usage(char **argv) {
  fprintf( stderr, "Usage: %s --threads=NTHREADS --iter=NINTERATIONS --yield=[ids] --sync=[ms] --lists=NLISTS\n", argv[0] );
  exit(-1);
}

pthread_mutex_t lock;
volatile int spinlock;

unsigned long long time_design;

struct arguments
{
  SortedList_t** listlist;
  int nLists;
  SortedListElement_t ** elements;
  int nIter,start;
};

void (*insert) (SortedList_t *list, SortedListElement_t *element);
int (*delete) (SortedListElement_t *element);
SortedListElement_t* (*lookup) (SortedList_t *list, const char *key);
int (*list_length) (SortedList_t *list);

/* Generate hash and return index of corresponding list */
int chooselist(const char * key, int nLists)
{
  int k = 111;
  long hash = 0;
  
  int i = 0;
  while(key[i] != 0) {
    hash += key[i] * pow(k, i);
    i++;
  }

  return hash % nLists;
}


void update_count(struct timespec begin, struct timespec end)
{
  unsigned long long value = (end.tv_sec*1e9 + end.tv_nsec) - (begin.tv_sec*1e9 + begin.tv_nsec);
  unsigned long long new = time_design + value;
  unsigned long long old = time_design;
  while(time_design != new) {
    old = time_design;
    new = time_design + value;
    __sync_val_compare_and_swap(&time_design, old, new);
  }
}

void *threadfunction(void*p)
{
  struct timespec begin, end;
  
  struct arguments values;
  values = *(struct arguments*) p;

  int k;
  int j;
  for (k = 0; k < values.nIter; k++)
    {
      clock_gettime(CLOCK_MONOTONIC, &begin);
      j = chooselist(values.elements[k]->key, values.nLists);
      insert(values.listlist[j], values.elements[k]);
      clock_gettime(CLOCK_MONOTONIC, &end);
      update_count(begin, end);
    }

  int length;
  for (k = 0; k < values.nLists; k++)
    {
      clock_gettime(CLOCK_MONOTONIC, &begin);
      length = list_length(values.listlist[k]);
      clock_gettime(CLOCK_MONOTONIC, &end);
      update_count(begin, end);
      if (length == -1)
	{
	  fprintf(stdout, "ERROR: SortedList_length returned %d\n", length);
	  exit(1);
	}
      //fprintf(stdout, "List length: %d\n", length);
    }

  SortedListElement_t* query;

  int status;
  status = 0;
  
  for (k = 0; k < values.nIter; k++)
    {
      clock_gettime(CLOCK_MONOTONIC, &begin);
      j = chooselist(values.elements[k]->key, values.nLists);
      query = lookup(values.listlist[j], values.elements[k]->key);
      status = delete(query);
      clock_gettime(CLOCK_MONOTONIC, &end);
      update_count(begin, end);
      if (status != 0)
	{
	  fprintf(stdout, "ERROR: SortedList_delete returned &d\n",status);
	  exit(1);
	}
    }
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
  int correct = 0;

  time_design = 0;
  struct timespec begin, end;  
  
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

    case 'c':
      correct = 1;
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

  SortedList_t ** listlist = malloc(sizeof(SortedList_t*) * nLists);
  for(i=0; i<nLists; i++) {
    listlist[i] = malloc(sizeof(SortedList_t));
    listlist[i]->next = listlist[i];
    listlist[i]->prev = listlist[i];
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
    //    printf("KEY%d: %s\n", i, keys[i]);
    elements[i]->key = keys[i];
  }

  pthread_t * threads = malloc(nThreads * sizeof(pthread_t*));

  struct arguments* parameters;
  parameters = malloc(nThreads * sizeof(struct arguments));

  
  clock_gettime(CLOCK_MONOTONIC, &begin);
  
  int t;
  int err;
  for(t = 0; t < nThreads; t++) {
    parameters[t].listlist = listlist;
    parameters[t].nLists = nLists;
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

  for(i=0; i<nLists; i++) {
    if (SortedList_length(listlist[i]) != 0) {
      fprintf(stderr, "Final length of list is not 0\n");
    }
  }

  unsigned long long nOps = nThreads*nIter*(nIter/nLists);
  time_design = correct ? time_design :
    (end.tv_sec*1e9 + end.tv_nsec) - (begin.tv_sec*1e9 + begin.tv_nsec);

  fprintf(stdout,
	  "%d threads x %d iterations x (ins + lookup/del) x (%d/2 avg len) = %d operations\n",
	  nThreads, nIter, nIter / nLists, nOps);
  fprintf(stdout, "elapsed time: %llu ns\n", time_design);
  fprintf(stdout, "per operation: %llu ns\n", time_design/nOps);
  
  free(threads);
  free(parameters);
  pthread_mutex_destroy(&lock);
  for(i=0; i<nElements; i++) {
    free(elements[i]);
    free(keys[i]);
  }
  for(i=0; i<nLists; i++)
    free(listlist[i]);
  free(elements);
  free(keys);
  free(listlist);
  
  return 0;
}
