#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <coresrv/vfs.h>
#include <coresrv/rootfs.h>
#include <coresrv/initfs.h>

struct vfs_rootfs vfs_rootfs;

void vfs_init() {
  vfs_rootfs_init(&vfs_rootfs);
}
