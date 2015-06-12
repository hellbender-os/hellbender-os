#include <glob.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

// functions here return one of three:
//  0            to continue with the current path name.
//  GLOB_SKIPOED to skip the current path name.
//  GLOB_ABORTED to stop globbing immediately.

#define GLOB_SKIPPED 100

#define RETURN_IF(x) { int _test = x; if (_test) return _test; }

// FAILURE reports error e, and returns _ABORTED or _SKIPPED
#define FAILURE(e) (((gs->errfunc && gs->errfunc(gs->path_start, e)) || gs->flags & GLOB_ERR) ? GLOB_ABORTED : GLOB_SKIPPED)

struct glob_state {
  int flags;
  int(*errfunc)(const char *epath, int eerrno);
  char *path_start; // start of the current path -buffer.
  char **next; // where to store the next path pointer.
  int n_paths; // how many paths were found.
};

typedef int (*void_compar)(const void*, const void*);

static int glob_sort(const char **d1, const char **d2) {
  return strcoll(*d1, *d2);
}

static char** split2(int n) {
  char *comp = strtok(NULL, "/");
  if (!comp) {
    char **array = (char**)malloc((n+1) * sizeof(char*));
    array[n] = 0;
    return array;
  } else {
    char **array = split2(n + 1);
    array[n] = comp;
    return array;
  }
}

static char** split(char *pattern) {
  char *comp = strtok(pattern, "/");
  char **array = split2(1);
  array[0] = comp;
  return array;
}

static bool is_pure(char *patt) {
  bool esc = false;
  for (; *patt; ++patt) {
    if (!esc) {
      if (*patt == '*') return false;
      if (*patt == '?') return false;
      if (*patt == '[') return false;
      if (*patt == '\\') esc = true;
    } else esc = false;
  }
  return true;
}

static int append(struct glob_state *gs, const char *comp, char **path_end) {
  int len = strlen(comp);
  int total_len = *path_end - gs->path_start + len + 1;
  if (total_len >= PATH_MAX) {
    return FAILURE(ENAMETOOLONG);
  } else {
    *path_end = stpcpy(*path_end, comp);
    return 0;
  }
}

static int append_and_store(struct glob_state *gs, const char *comp,
                            char *path_end) {
  RETURN_IF(append(gs, comp, &path_end));
  
  size_t len = path_end - gs->path_start + 1;
  void *ptr = malloc(len + sizeof(char*));
  char *path = (char*)(ptr + sizeof(char*));
  memcpy(path, gs->path_start, len);
  *gs->next = path;
  gs->next = (char**)ptr;
  gs->n_paths++;
  return 0;
}

int do_glob(struct glob_state *gs, char **comps, char *path_end) {
  char *comp = *comps++; // current component.
  char *next = *comps; // next (or NULL).
  bool pure = is_pure(comp);
  if (*next) { // iteratively process this directory component.
    // if pure, just append and recurse.
    if (pure) {
      RETURN_IF(append(gs, comp, &path_end));
      return do_glob(gs, comps, path_end);
      
    } else {
      // else, list all files and recurse.
      DIR *dir = opendir(gs->path_start);
      if (!dir) {
        return FAILURE(errno);
      }
      int retval = 0;
      for (struct dirent *de = readdir(dir); de; de = readdir(dir)) {
        if (!fnmatch(comp, de->d_name, gs->flags)) {
          if (do_glob(gs, comps, path_end) == GLOB_ABORTED) {
            retval = GLOB_ABORTED;
            break;
          }
        }
      }
      closedir(dir);
      return retval;
    }
    
  } else { // this is the file component.
    // TODO: append '/' to directories if GLOB_MARK is set.
    // TODO: if pure & exists, just append and store.
    // else, list all files and store each that match.
    DIR *dir = opendir(gs->path_start);
    if (!dir) {
      return FAILURE(errno);
    }
    int retval = 0;
    for (struct dirent *de = readdir(dir); de; de = readdir(dir)) {
      if (!fnmatch(comp, de->d_name, gs->flags)) {
        if (append_and_store(gs, de->d_name, path_end) == GLOB_ABORTED) {
          retval = GLOB_ABORTED;
          break;
        }
      }
    }
    closedir(dir);
    return retval;
  }
}

int glob(const char *pattern, int flags,
         int(*errfunc)(const char *epath, int eerrno),
         glob_t *pglob) {
  char path[PATH_MAX];
  char* path_end = path;
  if (pattern[0] == '/') {
    path[0] = '/';
    path[1] = 0;
    ++pattern;
    ++path_end;
  } else {
    path[0] = 0;
  }
  char* split_pat = strdup(pattern); // slashes will be replaced by zeros.
  char **comps = split(split_pat);

  // start iteratively building a path that consists of:
  // - pure components as such.
  // - directories found by matching impure components.
  // after adding the last component:
  // - allocate a buffer for the name + pointer to next name.
  // - copy the path to the buffer.
  // - set the new buffer into the last pointer.
  char *first_file = NULL;
  struct glob_state gs = (struct glob_state) {
    .flags = flags,
    .errfunc = errfunc,
    .path_start = path,
    .next = &first_file,
    .n_paths = 0
  };
  int retval = do_glob(&gs, comps, path_end);
  free(split_pat);

  // handle GLOB_NOCHECK
  if (gs.n_paths == 0) {
    if (!(flags & GLOB_NOCHECK)) {
      if (!(flags & GLOB_APPEND)) pglob->gl_pathc = 0;
      return GLOB_NOMATCH;
    } else {
      append_and_store(&gs, pattern, path);
    }
  }

  // reallocate enough pathv space.
  int n_null = 0;
  int idx = 0;
  if (flags & GLOB_DOOFFS) {
    n_null = pglob->gl_offs;
  } else pglob->gl_offs = 0;
  if (flags & GLOB_APPEND) {
    idx = n_null + pglob->gl_pathc;
    n_null = 0;
    pglob->gl_pathc += gs.n_paths;
  } else {
    idx = n_null;
    pglob->gl_pathc = gs.n_paths;
  }
  int n_total = idx + gs.n_paths;
  if (n_total) {
    if (flags & GLOB_APPEND) {
      pglob->gl_pathv =
        (char**)realloc(pglob->gl_pathv, n_total * sizeof(char*));
    } else {
      pglob->gl_pathv = (char**)malloc(n_total * sizeof(char*));
    }
  }

  // merge results.
  for (int i = 0; i < n_null; ++i) {
    pglob->gl_pathv[i] = NULL;
  }
  char** ptr = &first_file;
  for (int i = 0; i < gs.n_paths; ++i) {
    // paths and linked list pointers are co-allocated.
    // next pointer is just before the path.
    char *path = *ptr;
    ptr = (char**)(path - sizeof(char*));
    pglob->gl_pathv[idx + i] = path;
  }

  // optional sorting:
  if (!(flags & GLOB_NOSORT)) {
    qsort(pglob->gl_pathv + idx, gs.n_paths,
          sizeof(char**), (void_compar)glob_sort);
  }

  // TODO: check for out-of-memory (GLOB_NOSPACE)
  if (retval == GLOB_ABORTED) return GLOB_ABORTED;
  else return 0;
}
