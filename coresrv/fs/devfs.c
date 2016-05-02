#include <hellbender/fs/devfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

static devfs_t devfs;

int devfs_create(vfs_tag_t *root) {
  (void)root;
  return -1;
}

int devfs_add_cdev(const char *name, cdev_t *dev) {
  (void)name;
  (void)dev;
  return -1;
}

int devfs_add_bdev(const char *name, bdev_t *dev) {
  (void)name;
  (void)dev;
  return -1;
}

void devfs_bind(devfs_t *devfs_) {
  *devfs_ = devfs;
}

void devfs_init() {
  BROKER_REGISTER(DEVFS_ID, REGISTER_SERVICE(devfs_bind));
  devfs.create = REGISTER_SERVICE(devfs_create);
  devfs.add_cdev = REGISTER_SERVICE(devfs_add_cdev);
  devfs.add_bdev = REGISTER_SERVICE(devfs_add_bdev);
}
