#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <hellbender/fs/vfs.h>

char *realpath(const char *file_name, char *resolved_name) {
  vfs_file_t file;
  errno = vfs.open(0, file_name, 0, 0, 0, &file);
  if (errno) return 0;
  char *alloc_name = 0;
  if (!resolved_name) {
    resolved_name = alloc_name = malloc(PATH_MAX);
  }
  errno = vfs.get_path(&file, resolved_name, PATH_MAX);
  if (errno && alloc_name) {
    free(alloc_name);
    resolved_name = alloc_name = 0;
  }
  return errno ? 0 : resolved_name;
}

