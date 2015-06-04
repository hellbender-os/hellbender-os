#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../fcntl/fcntl_impl.h"

int fstat(int handle, struct stat *buf) {
  if (handle < 0 || handle >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.fstat) {
    *buf = file->stat;
  } else if (IDC(vfs_fstat, file->filesys.fstat, file, buf) != 0) {
    return -1;
  }
  return 0;
}
