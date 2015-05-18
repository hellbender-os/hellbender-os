#include <stdio.h>
#include <stdlib.h>
#include <coresrv/dev.h>
#include <coresrv/vfs.h>
#include <coresrv/devfs.h>

struct vfs_devfs dev_devfs;

void dev_init() {
  vfs_devfs_init(&dev_devfs);
  printf("Mounting devfs.\n");
  if (vfs_mount(NO_IDC, "devfs/", &dev_devfs.filesys)) {
    printf("Cannot mount dev file system.\n");
    abort();
  }
}
