#ifndef __HELLBENDER_KERNEL_LIST_H__
#define __HELLBENDER_KERNEL_LIST_H__

#include "config.h"
#include <stddef.h>

typedef struct list_item {
  struct list_item *prev;
  struct list_item *next;
} list_item_t;

typedef struct list {
  struct list_item head;
} list_t;

#define LIST_INIT {{0}}

INLINE void list_insert(list_t *list, list_item_t *item) {
  if (list->head.next) list->head.next->prev = item;
  item->next = list->head.next;
  item->prev = &list->head;
  list->head.next = item;
}

INLINE list_item_t* list_remove(list_item_t *item) {
  item->prev->next = item->next;
  if (item->next) item->next->prev = item->prev;
  return item->next;
}

INLINE list_item_t* list_first(list_t *list) {
  return list->head.next;
}

INLINE list_item_t* list_next(list_item_t *item) {
  return item->next;
}

#define list_container(ptr, type, member) (type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member))
                                       
#endif
