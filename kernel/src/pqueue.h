#ifndef __HELLBENDER_KERNEL_PQUEUE_H__
#define __HELLBENDER_KERNEL_PQUEUE_H__

#include "config.h"

#include <hellbender/list.h>

#include <stddef.h>

typedef list_item_t pqueue_item_t;

typedef struct pqueue {
  uint64_t available; // bit 1 => priority list not empty.
  list_t priorities[PRIORITY_LIMIT];
} pqueue_t;

// return the highest priority available; 0 means pqueue is empty.
INLINE unsigned pqueue_priority(pqueue_t *pqueue) {
  return pqueue->available == 0 ? 0 : 63 - __builtin_clzll(pqueue->available);
}

INLINE int pqueue_empty(pqueue_t *pqueue) {
  return pqueue->available == 0;
}

INLINE void pqueue_push(pqueue_t *pqueue, pqueue_item_t *item, unsigned priority) {
  list_t *list = pqueue->priorities + priority;
  list_insert(list, item);
  pqueue->available |= 1ull << priority % 64;
}

INLINE pqueue_item_t* pqueue_pop(pqueue_t *pqueue) {
  unsigned priority = pqueue_priority(pqueue);
  list_t *list = pqueue->priorities + priority;
  pqueue_item_t *item = list_first(list);
  if (item) {
    if (!list_remove(item)) {
      pqueue->available &= ~(1ull << priority % 64);
    }
  }
  return item;
}

INLINE void pqueue_remove(pqueue_t *pqueue, pqueue_item_t *item, unsigned priority) {
  if (item->prev && !list_remove(item)) {
    pqueue->available &= ~(1ull << priority % 64);
  }
  item->prev = 0;
}

#define pqueue_container(ptr, type, member) (type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member))
                                       
#endif
