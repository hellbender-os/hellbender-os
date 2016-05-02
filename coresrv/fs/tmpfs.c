#include <hellbender/fs/tmpfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

static tmpfs_t tmpfs;

int tmpfs_create(size_t max_size, vfs_tag_t *root) {
  (void)max_size;
  (void)root;
  return -1;
}

void tmpfs_bind(tmpfs_t *tmpfs_) {
  *tmpfs_ = tmpfs;
}

void tmpfs_init() {
  BROKER_REGISTER(TMPFS_ID, REGISTER_SERVICE(tmpfs_bind));
  tmpfs.create = REGISTER_SERVICE(tmpfs_create);
}
