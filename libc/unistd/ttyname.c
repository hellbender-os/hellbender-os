#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "../fcntl/fcntl_impl.h"

char *ttyname(int handle) {
  if (handle < 0 || handle >= OPEN_MAX) {
    errno = EBADF;
    return NULL;
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return NULL;
  }
  if (!file->filesys.termios) {
    errno = ENOTTY;
    return NULL;
  }
  return file->real_path;
}

int ttyname_r(int fildes, char *name, size_t namesize) {
  char *path = ttyname(fildes);
  if (!path) return -1;
  size_t pathlen = strlen(path);
  if (namesize < pathlen + 1) {
    errno = ERANGE;
    return -1;
  }
  memcpy(name, path, pathlen);
  name[pathlen] = 0;
  return 0;
}
