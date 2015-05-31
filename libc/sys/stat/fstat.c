#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../fcntl/fcntl_impl.h"

int fstat(int handle, struct stat *buf) {
  if (handle < 0 || handle >= OPEN_MAX) {
    printf("Illegal file handle.\n");
    abort();
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->filesys.open) {
    printf("File handle not open.\n");
    abort();
  }
  if (!file->filesys.fstat) {
    printf("fstat not supported.\n");
    abort();
  }
  return IDC(vfs_fstat, file->filesys.fstat, file, buf);
  
}
