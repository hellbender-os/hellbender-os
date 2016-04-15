#ifndef __HELLBENDER_LIST_H__
#define __HELLBENDER_LIST_H__

#include <stddef.h>

struct list_item {
  struct list_item *prev;
  struct list_item *next;
  int valid;
};

typedef struct list {
  struct list_item head;
  struct list_item tail;
} list_t;

void list_init(struct list *list);
void list_insert(struct list *list, struct list_item *item);
void list_remove(struct list_item *item);
void* list_next(struct list_item *item, int offset);
void* list_first(struct list *list, int offset);
void list_free_all(struct list *list, int offset);

#define LIST_ITEM struct list_item _list_item
#define LIST_INSERT(list, item) list_insert(list, &(item)->_list_item)
#define LIST_REMOVE(item) list_remove(&(item)->_list_item)
#define LIST_FIRST(list, type) ((type*)(list_first(list, offsetof(type, _list_item))))
#define LIST_NEXT(item, type) ((type*)(list_next(&(item)->_list_item, - offsetof(type, _list_item))))
#define LIST_FREE_ALL(list, type) list_free_all(list, offsetof(type, _list_item))
                                       
#endif
