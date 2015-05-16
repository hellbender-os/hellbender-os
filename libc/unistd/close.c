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
  vfo_t *vfo = &fcntl_data.handles[handle];
  if (!vfo->id) {
    printf("File handle not open.\n");
    abort();
  }
  if (!vfo->close) {
    printf("Close not supported.\n");
    abort();
  }
  printf("making call to vfo->close\n");
  return IDC(coresrv_vfs_close, vfo->close, vfo);
}

