#define _GNU_SOURCE
#include "SortedList.h"
#include <string.h>
#include <pthread.h>

int opt_yield;
volatile int spinlock;

void SortedList_insert_s(SortedList_t *list,
		       SortedListElement_t *element) {
  while(__sync_lock_test_and_set(&spinlock,1))
    continue;
  
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

  __sync_lock_release(&spinlock);
}

int SortedList_delete_s( SortedListElement_t *element) {
  while(__sync_lock_test_and_set(&spinlock,1))
    continue;

  SortedListElement_t *n = element->next;
  SortedListElement_t *p = element->prev;

  if(n->prev != element)
  {
    __sync_lock_release(&spinlock);
    return 1;
  }
  if(p->next != element)
  {
    __sync_lock_release(&spinlock);
    return -1;
  }

  if (opt_yield & DELETE_YIELD)
    pthread_yield();

  n->prev = p;
  p->next = n;
  element->next = NULL;
  element->prev = NULL;

  __sync_lock_release(&spinlock);
  
  return 0;
}

SortedListElement_t *SortedList_lookup_s(SortedList_t *list,
				       const char *key) {
  while(__sync_lock_test_and_set(&spinlock,1))
    continue;

  SortedListElement_t *n = list->next;

  if (opt_yield & SEARCH_YIELD)
    pthread_yield();

  while(strcmp(n->key, key) != 0) {
    if (n == list)
    {
      __sync_lock_release(&spinlock);
      return NULL;
    }
    n = n->next;
  }
  __sync_lock_release(&spinlock);
  return n;
}

int SortedList_length_s(SortedList_t *list) {
  while(__sync_lock_test_and_set(&spinlock,1))
    continue;

  int count = 0;
  SortedListElement_t *c = list->next;

  if (opt_yield & SEARCH_YIELD)
    pthread_yield();

  while(c != list) {
    SortedListElement_t *n = c->next;
    SortedListElement_t *p = c->prev;
    if(n->prev != c)
    {
      __sync_lock_release(&spinlock);
      return -1;
    }
    if(p->next != c)
    {
      __sync_lock_release(&spinlock);
      return -1;
    }
    count++;
    c = c->next;
  }
  
  __sync_lock_release(&spinlock);
  return count;
}
