#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <hellbender/list.h>

int alphasort(const struct dirent **d1, const struct dirent **d2) {
  if (!*d1 || !*d2) return 0;
  return strcoll((*d1)->d_name, (*d2)->d_name);
}

struct dirent_ll {
  struct dirent de;
  list_item_t item;
};

typedef int (*void_compar)(const void*, const void*);

int scandir(const char *dirname, struct dirent ***namelist,
            int (*sel)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
  if (!compar) {
    errno = EINVAL;
    return -1;
  }
  // scan the directory, collect all selected entries into a linked list.
  list_t accepted;
  int n_accepted = 0;
  DIR *dir = opendir(dirname);
  if (!dir) {
    return -1;
  }
  struct dirent_ll *next = malloc(sizeof(*next));
  while (1) {
    struct dirent *ptr = NULL;
    if (readdir_r(dir, &next->de, &ptr) != 0) {
      n_accepted = -1;
      break;
    }
    if (!ptr) {
      break;
    }
    if (!sel || (*sel)(&next->de)) {
      ++n_accepted;
      list_insert(&accepted, &next->item);
      next = malloc(sizeof(*next));
    }
  }
  free(next);

  // put all selected entries into namelist array and sort.
  if (n_accepted >= 0) {
    *namelist = malloc(sizeof(struct dirent*) * n_accepted);
    int idx = 0;
    for (list_item_t *li = list_first(&accepted); li; li = list_next(li)) {
      struct dirent_ll *dell = list_container(li, struct dirent_ll, item);
      *namelist[idx++] = &dell->de;
    }
    qsort(*namelist, n_accepted, sizeof(struct dirent*), (void_compar)compar);
  }

  closedir(dir);
  for (list_item_t *li = list_first(&accepted); li; ) {
    struct dirent_ll *dell = list_container(li, struct dirent_ll, item);
    li = list_next(li);
    free(dell);
  }
  return n_accepted;
}
