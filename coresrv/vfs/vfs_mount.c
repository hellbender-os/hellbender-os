#include <coresrv/vfs.h>
#include <coresrv/rootfs.h>

__IDCIMPL__ int vfs_mount(IDC_PTR, const char* name, struct vfs_filesys *filesys) {
  return vfs_rootfs_add(&vfs_rootfs, name, filesys);
}

