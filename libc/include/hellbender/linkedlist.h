#ifndef _HELLBENDER_LINKEDLIST_H
#define _HELLBENDER_LINKEDLIST_H

#include <stddef.h>

struct _llitem {
  struct _llitem *prev;
  struct _llitem *next;
  int valid;
};

struct _linkedlist {
  struct _llitem head;
  struct _llitem tail;
};

void _ll_init(struct _linkedlist *list);
void _ll_insert(struct _linkedlist *list, struct _llitem *item);
void _ll_remove(struct _llitem *item);
void* _ll_next(struct _llitem *item, int offset);
void* _ll_first(struct _linkedlist *list, int offset);
void _ll_free_all(struct _linkedlist *list, int offset);

#define _LL_ITEM struct _llitem _ll
#define _LL_INSERT(list, item) _ll_insert(list, &item->_ll)
#define _LL_REMOVE(item) _ll_remove(&item->_ll)
#define _LL_FIRST(list, type) ((type*)(_ll_first(list, offsetof(type, _ll))))
#define _LL_NEXT(item, type) ((type*)(_ll_next(&item->_ll, - offsetof(type, _ll))))
#define _LL_FREE_ALL(list, type) _ll_free_all(list, offsetof(type, _ll))
                                       
#endif
