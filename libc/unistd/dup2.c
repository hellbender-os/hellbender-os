#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "../fcntl/fcntl_impl.h"

int dup(int fildes) {
  return fcntl(fildes, F_DUPFD, 0);
}

int dup2(int old_fd, int new_fd) {
  if (old_fd < 0 || old_fd >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  if (new_fd < 0 || new_fd >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *old_file = &_fcntl_data.handles[old_fd];
  if (!old_file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *new_file = &_fcntl_data.handles[new_fd];
  if (new_file->stat.st_mode) {
    close(new_fd);
  }
  *new_file = *old_file;
  return new_fd;
}
