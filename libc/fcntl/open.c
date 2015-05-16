#include <stdio.h>
#include <fcntl.h>
#include <kernel/kernel.h>

#include "fcntl_impl.h"

int open(const char *name, int flags, ...) {
  // allocate virtual file object.
  int handle = fcntl_find_handle();
  if (handle < 0) {
    printf("Error: out of file handles.\n");
    return -1;
  }
  // ask virtual file system to populate it for the file.
  vfo_t *vfo = &fcntl_data.handles[handle];
  //printf("making call to vfs_open\n");
  BREAK;
  if (CORE_IDC(coresrv_vfs_open, vfo, name, flags) == 0) {
    return handle;
  } else {
    printf("Error: failed to open '%s'\n", name);
  }
}
