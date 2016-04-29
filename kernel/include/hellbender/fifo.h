#ifndef __HELLBENDER_KERNEL_FIFO_H__
#define __HELLBENDER_KERNEL_FIFO_H__

#include "config.h"
#include <stddef.h>

typedef struct fifo_item {
  struct fifo_item *next;
} fifo_item_t;

typedef struct fifo {
  struct fifo_item *head;
  struct fifo_item *tail;
} fifo_t;

INLINE void fifo_push(fifo_t *fifo, fifo_item_t *item) {
  item->next = 0;
  if (!fifo->head) fifo->head = item;
  if (fifo->tail) fifo->tail->next = item;
  fifo->tail = item;
}

INLINE fifo_item_t* fifo_pop(fifo_t *fifo) {
  fifo_item_t* item = fifo->head;
  if (item) {
    fifo->head = item->next;
    if (fifo->tail == item) fifo->tail = 0;
  }
  return item;
}

INLINE int fifo_empty(fifo_t *fifo) {
  return fifo->head == 0;
}

#define fifo_container(ptr, type, member) (type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member))
                                       
#endif
