#include <unistd.h>
#include <errno.h>

#include "../fcntl/fcntl_impl.h"

int isatty(int fildes) {
  if (fildes < 0 || fildes >= OPEN_MAX) {
    errno = EBADF;
    return 0;
  }
  struct vfs_file *file = &_fcntl_data.handles[fildes];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return 0;
  }
  if (!file->isatty) {
    errno = ENOTTY;
    return 0;
  }
  return 1;
}

