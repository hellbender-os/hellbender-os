#include <hellbender/fs/initfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

static initfs_t initfs;

int initfs_create(void *initrd_base, size_t initrd_size, vfs_tag_t *root) {
  (void)initrd_base;
  (void)initrd_size;
  (void)root;
  return -1;
}

void initfs_bind(initfs_t *initfs_) {
  *initfs_ = initfs;
}

void initfs_init() {
  BROKER_REGISTER(INITFS_ID, REGISTER_SERVICE(initfs_bind));
  initfs.create = REGISTER_SERVICE(initfs_create);
}
