#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "../fcntl/fcntl_impl.h"

int fchdir(int fildes) {
  if (fildes < 0 || fildes >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *dir = &_fcntl_data.handles[fildes];
  if (!dir->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!S_ISDIR(dir->stat.st_mode)) {
    errno = ENOTDIR;
    return -1;
  }
  return chdir(dir->real_path);
}

