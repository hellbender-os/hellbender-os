#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

off_t lseek(int handle, off_t off, int where) {
  if (where != SEEK_SET && where != SEEK_CUR && where != SEEK_END) {
    errno = EINVAL;
    return -1;
  }
  if (handle < 0 || handle >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.lseek) {
    errno = EBADF;
    return -1;
  }
  return IDC(vfs_lseek, file->filesys.lseek, file, off, where);
}
