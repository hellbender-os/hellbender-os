#include <coresrv/dev.h>
#include <coresrv/devfs.h>

extern struct vfs_devfs dev_devfs;

__IDCIMPL__ int dev_register(IDC_PTR, const char* name, struct vfs_filesys* filesys) {
  return vfs_devfs_add(&dev_devfs, name, filesys);
}
