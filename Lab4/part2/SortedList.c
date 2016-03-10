#define _GNU_SOURCE
#include "SortedList.h"
#include <string.h>
#include <pthread.h>

int opt_yield;

void SortedList_insert(SortedList_t *list,
		       SortedListElement_t *element) {
  SortedListElement_t *p = list;
  SortedListElement_t *n = list->next;
  while(n != list) {
    if(strcmp(element->key, n->key) <= 0)
      break;
    p = n;
    n = n->next;
  }

  if (opt_yield & INSERT_YIELD)
    pthread_yield();
  
  element->prev = p;
  element->next = n;
  p->next = element;
  n->prev = element;
}

int SortedList_delete( SortedListElement_t *element) {
  SortedListElement_t *n = element->next;
  SortedListElement_t *p = element->prev;

  if(n->prev != element)
    return 1;
  if(p->next != element)
    return -1;

  if (opt_yield & DELETE_YIELD)
    pthread_yield();
  
  n->prev = p;
  p->next = n;
  element->next = NULL;
  element->prev = NULL;
  
  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list,
				       const char *key) {
  SortedListElement_t *n = list->next;

  if (opt_yield & SEARCH_YIELD)
    pthread_yield();
  
  while(strcmp(n->key, key) != 0) {
    if (n == list)
      return NULL;

    n = n->next;
  }
  return n;
}

int SortedList_length(SortedList_t *list) {
  int count = 0;
  SortedListElement_t *c = list->next;
  
  if (opt_yield & SEARCH_YIELD)
    pthread_yield();

  while(c != list) {
    SortedListElement_t *n = c->next;
    SortedListElement_t *p = c->prev;
    if(n->prev != c)
      return -1;
    if(p->next != c)
      return -1;
    
    count++;
    c = c->next;
  }

  return count;
}
