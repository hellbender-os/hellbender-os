#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <hellbender/linkedlist.h>

int alphasort(const struct dirent **d1, const struct dirent **d2) {
  if (!*d1 || !*d2) return 0;
  return strcoll((*d1)->d_name, (*d2)->d_name);
}

struct dirent_ll {
  struct dirent de;
  _LL_ITEM;
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
  struct _linkedlist accepted;
  unsigned n_accepted = 0;
  DIR *dir = opendir(dirname);
  struct dirent_ll *next = (struct dirent_ll*)malloc(sizeof(struct dirent_ll));
  while (1) {
    struct dirent *ptr = NULL;
    if (readdir_r(dir, &next->de, &ptr) != 0) {
      goto error;
    }
    if (!ptr) {
      break;
    }
    if (!sel || (*sel)(&next->de)) {
      ++n_accepted;
      _LL_INSERT(&accepted, next);
      next = (struct dirent_ll*)malloc(sizeof(struct dirent_ll));
    }
  }
  free(next);

  // put all selected entries into namelist array and sort.
  *namelist = (struct dirent**)malloc(sizeof(struct dirent*) * n_accepted);
  int idx = 0;
  for (struct dirent_ll *dell = _LL_FIRST(&accepted, struct dirent_ll);
       dell; dell = _LL_NEXT(dell, struct dirent_ll)) {
    *namelist[idx++] = &dell->de;
  }
  qsort(*namelist, n_accepted, sizeof(struct dirent*), (void_compar)compar);
  return n_accepted;

 error:
  closedir(dir);
  free(next);
  _LL_FREE_ALL(&accepted, struct dirent_ll);
  return -1;
}
