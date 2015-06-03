#include <hellbender/linkedlist.h>

void _ll_init(struct _linkedlist *list) {
  list->head.prev = NULL;
  list->head.next = &list->tail;
  list->tail.prev = &list->head;
  list->tail.next = NULL;
  list->head.valid = 0;
  list->tail.valid = 0;
}

void _ll_insert(struct _linkedlist *list, struct _llitem *item) {
  item->valid = 1;
  item->prev = &list->head;
  item->next = list->head.next;
  list->head.next->prev = item;
  list->head.next = item;
}

void _ll_remove(struct _llitem *item) {
  item->prev->next = item->next;
  item->next->prev = item->prev;
  item->prev = NULL;
  item->next = NULL;
  item->valid = 0;
}

void* _ll_next(struct _llitem *item, int offset) {
  if (!item ||!item->next) return NULL;
  item = item->next;
  if (!item->valid) return NULL;
  return ((void*)item) - offset;
}

void* _ll_first(struct _linkedlist *list, int offset) {
  return _ll_next(&list->head, offset);
}
