#include <hellbender/rbtree.h>

static rbtree_item_t *binary_insert(rbtree_t *tree, rbtree_item_t *z) {
  rbtree_item_t *x = tree->root.left;
  rbtree_item_t *y = &tree->root;
  rbtree_item_t *nil = &tree->nil;
  rbtree_item_t* *ptr = &y->left;
  
  z->left = z->right = nil;
  while (x != nil) {
    y = x;
    int test = tree->compare(x, z);
    if (test > 0) { /* x > z */
      ptr = &x->left;
      x = x->left;
    } else if (test < 0) { /* x <= z */
      ptr = &x->right;
      x = x->right;
    } else { /* x == z */
      return x;
    }
  }
  z->parent = y;
  *ptr = z;
  return 0;
}

void left_rotate(rbtree_t *tree, rbtree_item_t *x) {
  rbtree_item_t *y;
  rbtree_item_t *nil = &tree->nil;

  y = x->right;
  x->right = y->left;
  if (y->left != nil) y->left->parent = x;
  y->parent = x->parent;   
  if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree_t *tree, rbtree_item_t *y) {
  rbtree_item_t *x;
  rbtree_item_t *nil = &tree->nil;

  x = y->left;
  y->left = x->right;
  if (nil != x->right) x->right->parent = y;
  x->parent = y->parent;
  if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}

rbtree_item_t *rbtree_insert(rbtree_t *tree, rbtree_item_t *item) {
  rbtree_item_t *old = binary_insert(tree, item);
  if (old) return old;

  rbtree_item_t *y;
  rbtree_item_t *x = item;
  x->red = 1;
  while (x->parent->red) { /* use sentinel instead of checking for root */
    if (x->parent == x->parent->parent->left) {
      y = x->parent->parent->right;
      if (y->red) {
	x->parent->red = 0;
	y->red = 0;
	x->parent->parent->red = 1;
	x = x->parent->parent;
      } else {
	if (x == x->parent->right) {
	  x = x->parent;
	  left_rotate(tree, x);
	}
	x->parent->red = 0;
	x->parent->parent->red = 1;
	right_rotate(tree, x->parent->parent);
      } 
    } else { /* case for x->parent == x->parent->parent->right */
      y = x->parent->parent->left;
      if (y->red) {
	x->parent->red = 0;
	y->red = 0;
	x->parent->parent->red = 1;
	x = x->parent->parent;
      } else {
	if (x == x->parent->left) {
	  x = x->parent;
	  right_rotate(tree, x);
	}
	x->parent->red = 0;
	x->parent->parent->red = 1;
	left_rotate(tree, x->parent->parent);
      }
    }
  }
  tree->root.left->red = 0;
  return 0;
}

void rbtree_replace(rbtree_item_t *old_item, rbtree_item_t *new_item) {
  new_item->red = old_item->red;
  new_item->left = old_item->left;
  new_item->right = old_item->right;
  rbtree_item_t *p = new_item->parent = old_item->parent;
  if (p->left == old_item) p->left = new_item;
  else p->right = new_item;
}

static rbtree_item_t* rbtree_successor(rbtree_t *tree, rbtree_item_t *x) { 
  rbtree_item_t *y;
  rbtree_item_t *nil = &tree->nil;
  rbtree_item_t *root = &tree->root;

  if (nil != x->right) {
    y = x->right;
    while (y->left != nil) { /* returns the minium of the right subtree of x */
      y = y->left;
    }
    return y;
  } else {
    y = x->parent;
    while (x == y->right) { /* sentinel used instead of checking for nil */
      x = y;
      y = y->parent;
    }
    if (y == root) return nil;
    return y;
  }
}

static void delete_fixup(rbtree_t *tree, rbtree_item_t *x) {
  rbtree_item_t *root = tree->root.left;
  rbtree_item_t *w;

  while ((!x->red) && (root != x)) {
    if (x == x->parent->left) {
      w = x->parent->right;
      if (w->red) {
	w->red = 0;
	x->parent->red = 1;
	left_rotate(tree, x->parent);
	w = x->parent->right;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
	w->red = 1;
	x = x->parent;
      } else {
	if (!w->right->red) {
	  w->left->red = 0;
	  w->red = 1;
	  right_rotate(tree, w);
	  w = x->parent->right;
	}
	w->red = x->parent->red;
	x->parent->red = 0;
	w->right->red = 0;
	left_rotate(tree, x->parent);
	x = root; /* this is to exit while loop */
      }
    } else { /* the code below has left and right switched from above */
      w = x->parent->left;
      if (w->red) {
	w->red = 0;
	x->parent->red = 1;
	right_rotate(tree, x->parent);
	w = x->parent->left;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
	w->red = 1;
	x = x->parent;
      } else {
	if (!w->left->red) {
	  w->right->red = 0;
	  w->red = 1;
	  left_rotate(tree, w);
	  w = x->parent->left;
	}
	w->red = x->parent->red;
	x->parent->red = 0;
	w->left->red = 0;
	right_rotate(tree, x->parent);
	x = root; /* this is to exit while loop */
      }
    }
  }
  x->red = 0;
}

void rbtree_erase(rbtree_t *tree, rbtree_item_t *z) {
  rbtree_item_t *y;
  rbtree_item_t *x;
  rbtree_item_t *nil = &tree->nil;
  rbtree_item_t *root = &tree->root;

  y = ((z->left == nil) || (z->right == nil)) ? z : rbtree_successor(tree, z);
  x = (y->left == nil) ? y->right : y->left;
  x->parent = y->parent;
  if (root == y->parent) {
    root->left = x;
  } else {
    if (y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }
  if (y != z) { /* z has two children, replace it by its successor (y). */
    if (!(y->red)) delete_fixup(tree, x);
    y->left = z->left;
    y->right = z->right;
    y->parent = z->parent;
    y->red = z->red;
    z->left->parent = z->right->parent = y;
    if (z == z->parent->left) {
      z->parent->left = y; 
    } else {
      z->parent->right = y;
    }
  } else { /* z has just one children, trivial to remove. */
    if (!(y->red)) delete_fixup(tree, x);
  }
}

rbtree_item_t *rbtree_find(rbtree_t *tree, rbtree_item_t *q) {
  rbtree_item_t* x = tree->root.left;
  rbtree_item_t* nil = &tree->nil;
  while (x != nil) {
    int compVal = tree->compare(x, q);
    if (compVal > 0) { /* x > q */
      x = x->left;
    } else if (compVal < 0) {
      x = x->right;
    } else return x;
  }
  return 0;
}

void rbtree_init(rbtree_t *tree, rbtree_comp_t compare) {
  tree->compare = compare;
  rbtree_item_t *nil = &tree->nil;
  nil->parent = nil->left = nil->right = nil;
  nil->red = 0;

  rbtree_item_t *root = &tree->root;
  root->parent = root->left = root->right = nil;
  root->red = 0;
}
