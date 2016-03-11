#define _GNU_SOURCE
#include "SortedList.h"
#include <string.h>
#include <pthread.h>

int opt_yield;
pthread_mutex_t lock;

void SortedList_insert_m(SortedList_t *list,
		       SortedListElement_t *element) {
  pthread_mutex_lock(&lock);

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
  pthread_mutex_unlock(&lock);
}

int SortedList_delete_m( SortedListElement_t *element) {
  pthread_mutex_lock(&lock);
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
  pthread_mutex_unlock(&lock);
  return 0;
}

SortedListElement_t *SortedList_lookup_m(SortedList_t *list,
				       const char *key) {
  pthread_mutex_lock(&lock);
  SortedListElement_t *n = list->next;

  if (opt_yield & SEARCH_YIELD)
    pthread_yield();
  
  while(strcmp(n->key, key) != 0) {
    if (n == list) {
      pthread_mutex_unlock(&lock);
      return NULL;
    }
    n = n->next;
  }
  pthread_mutex_unlock(&lock);
  return n;
}

int SortedList_length_m(SortedList_t *list) {

  pthread_mutex_lock(&lock);
  int count = 0;
  SortedListElement_t *c = list->next;
  
  if (opt_yield & SEARCH_YIELD)
    pthread_yield();

  while(c != list) {
    SortedListElement_t *n = c->next;
    SortedListElement_t *p = c->prev;
    if(n->prev != c) {
      pthread_mutex_unlock(&lock);
      return -1;
    }
    if(p->next != c) {
      pthread_mutex_unlock(&lock);
      return -1;
    }
    
    count++;
    c = c->next;
  }

  pthread_mutex_unlock(&lock);
  return count;
}
