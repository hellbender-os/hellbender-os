#ifndef __HELLBENDER_DICT_H__
#define __HELLBENDER_DICT_H__

#include "inline.h"
#include "rbtree.h"

#include <stddef.h>

typedef struct dict_item {
  rbtree_item_t item;
  int key_len;
  const char *key;
} dict_item_t;

typedef struct dict {
  rbtree_t tree;
} dict_t;

int dict_rbtree_comp(rbtree_item_t *item1, rbtree_item_t *item2);

// returns 0 on success, existing item on failure.
INLINE dict_item_t *dict_insert(dict_t *dict, dict_item_t *item) {
  rbtree_item_t *old = rbtree_insert(&dict->tree, &item->item);
  if (old) {
    dict_item_t *oi = rbtree_container(old, dict_item_t, item);
    return oi;
  } else return 0;
}

// returns the replaced item, or 0 if no item existed.
INLINE dict_item_t *dict_replace(dict_t *dict, dict_item_t *item) {
  dict_item_t *old = dict_insert(dict, item);
  if (old) {
    rbtree_replace(&old->item, &item->item);
  }
  return old;
}

INLINE void dict_erase(dict_t *dict, dict_item_t *item) {
  rbtree_erase(&dict->tree, &item->item);
}

INLINE dict_item_t *dict_find(dict_t *dict, const char *key, int key_len) {
  dict_item_t di;
  di.key_len = key_len;
  di.key = key;
  rbtree_item_t *ptr = rbtree_find(&dict->tree, &di.item);
  if (ptr) { 
    dict_item_t *oi = rbtree_container(ptr, dict_item_t, item);
    return oi;
 } else return 0;
}

INLINE void dict_init(dict_t *dict) {
  rbtree_init(&dict->tree, dict_rbtree_comp);
}

#define dict_container(ptr, type, member) (type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member))

#endif
