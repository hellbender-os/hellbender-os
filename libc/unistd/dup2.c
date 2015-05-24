#include <unistd.h>
#include <stdio.h>
#include "../fcntl/fcntl_impl.h"

int dup2(int old_fd, int new_fd) {
  if (old_fd < 0 || old_fd >= OPEN_MAX) {
    printf("Illegal old file handle.\n");
    return -1;
  }
  if (new_fd < 0 || new_fd >= OPEN_MAX) {
    printf("Illegal new file handle.\n");
    return -1;
  }
  struct vfs_file *old_file = &_fcntl_data.handles[old_fd];
  if (!old_file->filesys.open) {
    printf("Old file handle not open.\n");
    return -1;
  }
  struct vfs_file *new_file = &_fcntl_data.handles[new_fd];
  if (new_file->filesys.open) {
    close(new_fd);
  }
  *new_file = *old_file;
  return new_fd;
}
