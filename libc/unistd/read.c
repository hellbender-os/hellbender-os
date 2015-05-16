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
  vfo_t *vfo = &fcntl_data.handles[handle];
  if (!vfo->id) {
    printf("File handle not open.\n");
    abort();
  }
  if (!vfo->read) {
    printf("Read not supported.\n");
    abort();
  }
  printf("making call to vfo->read\n");
  return IDC(coresrv_vfs_read, vfo->read, vfo, buffer, size);
}
