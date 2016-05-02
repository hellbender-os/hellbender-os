#ifndef __HELLBENDER_RBTREE_H__
#define __HELLBENDER_RBTREE_H__

#include <stddef.h>

typedef struct rbtree_item {
  int red;
  struct rbtree_item *left, *right, *parent;
} rbtree_item_t;

typedef int (*rbtree_comp_t)(rbtree_item_t *item1, rbtree_item_t *item2);

typedef struct rbtree {
  rbtree_comp_t compare;
  struct rbtree_item root, nil;
} rbtree_t;

// returns 0 on success, existing item on failure.
rbtree_item_t *rbtree_insert(rbtree_t *rbtree, rbtree_item_t *item);

// returns the replaced item, or 0 if no item existed.
void rbtree_replace(rbtree_item_t *old_item, rbtree_item_t *new_item);

void rbtree_erase(rbtree_t *rbtree, rbtree_item_t *item);

rbtree_item_t *rbtree_find(rbtree_t *rbtree, rbtree_item_t *item);

void rbtree_init(rbtree_t *rbtree, rbtree_comp_t compare);

#define rbtree_container(ptr, type, member) (type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member))

#endif
