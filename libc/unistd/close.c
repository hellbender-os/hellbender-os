#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

int close(int handle) {
  if (handle < 0 || handle >= OPEN_MAX) {
    printf("Illegal file handle.\n");
    abort();
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->filesys.open) {
    printf("File handle not open.\n");
    abort();
  }
  if (!file->filesys.close) {
    printf("Close not supported.\n");
    abort();
  }
  int retval = IDC(vfs_close, file->filesys.close, file);
  fcntl_release_handle(handle);
  return retval;
}

