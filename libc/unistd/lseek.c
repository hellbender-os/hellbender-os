#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

off_t lseek(int handle, off_t off, int where) {
  if (handle < 0 || handle >= OPEN_MAX) {
    printf("Illegal file handle.\n");
    abort();
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->filesys.open) {
    printf("File handle not open.\n");
    abort();
  }
  if (!file->filesys.lseek) {
    printf("Lseek not supported.\n");
    abort();
  }
  return IDC(vfs_lseek, file->filesys.lseek, file, off, where);
}
