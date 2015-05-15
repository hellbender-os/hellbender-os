#include <string.h>
#include <coresrv/vfs.h>
#include <coresrv/rootfs.h>

__IDCIMPL__ int coresrv_vfs_open(IDC_PTR, vfo_t* vfo, const char *name, int flags) {
  if (strlen(name) == 0) { // "" always means the "root filesystem".
    return coresrv_rootfs_open(NO_IDC, vfo, name, flags);
  }
  
  return -1;
}

