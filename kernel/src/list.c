#include "list.h"

void list_init(struct list *list) {
  list->head.prev = NULL;
  list->head.next = &list->tail;
  list->tail.prev = &list->head;
  list->tail.next = NULL;
  list->head.valid = 0;
  list->tail.valid = 0;
}

void list_insert(struct list *list, struct list_item *item) {
  item->valid = 1;
  item->prev = &list->head;
  item->next = list->head.next;
  list->head.next->prev = item;
  list->head.next = item;
}

void list_remove(struct list_item *item) {
  item->prev->next = item->next;
  item->next->prev = item->prev;
  item->prev = NULL;
  item->next = NULL;
  item->valid = 0;
}

void* list_next(struct list_item *item, int offset) {
  if (!item || !item->next) return NULL;
  item = item->next;
  if (!item->valid) return NULL;
  return ((void*)item) - offset;
}

void* list_first(struct list *list, int offset) {
  return list_next(&list->head, offset);
}

void list_free_all(struct list *list, int offset) {
  void* cur = list_first(list, offset);
  while (cur) {
    void* next = list_next(cur, offset);
    //free(cur);
    cur = next;
  }
}
