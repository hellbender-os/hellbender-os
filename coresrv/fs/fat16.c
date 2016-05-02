#include <hellbender/fs/fat16.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

static fat16_t fat16;

int fat16_create(bdev_t *dev, vfs_tag_t *root) {
  (void)dev;
  (void)root;
  return -1;
}

void fat16_bind(fat16_t *fat16_) {
  *fat16_ = fat16;
}

void fat16_init() {
  BROKER_REGISTER(FAT16_ID, REGISTER_SERVICE(fat16_bind));
  fat16.create = REGISTER_SERVICE(fat16_create);
}
