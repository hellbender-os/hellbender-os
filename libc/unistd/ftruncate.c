#include <unistd.h>
#include <errno.h>

#include "../fcntl/fcntl_impl.h"

int ftruncate(int fildes, off_t length) {
  if (length < 0) {
    errno = EINVAL;
    return -1;
  }
  if (fildes < 0 || fildes > OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[fildes];
  if (!file->stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  if (!file->filesys.ftruncate) {
    errno = EROFS;
    return -1;
  }
  if (IDC(vfs_ftruncate, file->filesys.ftruncate, file, length) != 0) {
    return -1;
  }
  return 0;
}
