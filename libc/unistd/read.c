#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

ssize_t read(int handle, void *buffer, size_t size) {
  if (handle < 0 || handle >= OPEN_MAX) {
    printf("Illegal file handle.\n");
    abort();
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->filesys.open) {
    printf("File handle not open.\n");
    abort();
  }
  if (!file->filesys.read) {
    printf("Read not supported.\n");
    abort();
  }
  return IDC(vfs_read, file->filesys.read, file, buffer, size);
}
