#include <coresrv/dev.h>
#include <coresrv/devfs.h>

extern struct dev_devfs vfs_devfs;

__IDCIMPL__ int dev_unregister(IDC_PTR, const char* name) {
  (void)(name);
  // TODO: impl
  return -1;
}
