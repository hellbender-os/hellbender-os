#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

int close(int handle) {
  if (handle < 0 || handle >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.close) {
    errno = EBADF;
    return -1;
  }
  int retval = IDC(vfs_close, file->filesys.close, file);
  file->stat.st_mode = 0;
  fcntl_release_handle(handle);
  return retval;
}

