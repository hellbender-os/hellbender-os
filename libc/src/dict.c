#include <hellbender/dict.h>

static int dict_rbtree_comp(rbtree_item_t *item1, rbtree_item_t *item2) {
  dict_item_t *di1 = rbtree_container(item1, dict_item_t, item);
  dict_item_t *di2 = rbtree_container(item2, dict_item_t, item);
  int len1 = di1->key_len;
  int len2 = di2->key_len;
  if (len1 < len2) return -1;
  else if (len2 < len1) return +1;
  else { // len1 == len2
    const char *b1 = di1->key;
    const char *b2 = di2->key;
    const char *e1 = b1 + len1;
    const char *e2 = b2 + len1;
    for (int i = (len1+1)/2; i > 0; --i, ++b1, ++b2, --e1, --e2) {
      if (*b1 < *b2) return -1;
      else if (*b2 < *b1) return +1;
      else if (*e1 < *e2) return -1;
      else if (*e2 < *e1) return +1;
    }
    return 0;
  }
}

void dict_init(dict_t *dict) {
  rbtree_init(&dict->tree, dict_rbtree_comp);
}
