#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

ssize_t write(int handle, const void *buffer, size_t size) {
  if (handle < 0 || handle >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.write) {
    errno = EROFS;
    return -1;
  }
  return IDC(vfs_write, file->filesys.write, file, buffer, size);
}

ssize_t pwrite(int handle, const void *buf, size_t nbyte, off_t offset) {
  off_t current = lseek(handle, 0, SEEK_CUR);
  lseek(handle, offset, SEEK_SET);
  ssize_t retval = write(handle, buf, nbyte);
  lseek(handle, current, SEEK_SET);
  return retval;
}
