#include <glob.h>
#include <stddef.h>
#include <stdlib.h>

void globfree(glob_t *pglob) {
  if (!pglob) return;
  for (unsigned i = 0; pglob->gl_pathv && i < pglob->gl_pathc; ++i) {
    char *path = pglob->gl_pathv[pglob->gl_offs + i];
    // path + linked list pointers are coallocated:
    char *ptr = path - sizeof(char*);
    free(ptr);
  }
  free(pglob->gl_pathv);
  pglob->gl_pathv = NULL;
}

