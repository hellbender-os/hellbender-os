#include <hellbender/dev/ramdisk.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

static ramdisk_t ramdisk;

int ramdisk_create(size_t size, bdev_t *dev) {
  (void)size;
  (void)dev;
  return -1;
}

void ramdisk_bind(ramdisk_t *ramdisk_) {
  *ramdisk_ = ramdisk;
}

void ramdisk_init() {
  BROKER_REGISTER(RAMDISK_ID, REGISTER_SERVICE(ramdisk_bind));
  ramdisk.create = REGISTER_SERVICE(ramdisk_create);
}
