#include <hellbender/dev/ramdisk.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>

// bdev_t::impl.data == struct ramdisk_instance
// vfs_tag_t::impl.data == bdev_t
// vfs_node_t::impl.data == 0 // TODO: do we need buffer for open ramdisk devices?

struct ramdisk_instance {
  // TODO: allocated memory.
};

static struct ramdisk_data {
  ramdisk_t ramdisk;
  struct vfs_tag_op tag_op;
  struct vfs_node_op node_op;
  struct bdev_op bdev_op;
  // TODO: list of ramdisk_instances?
} ramdisk_data;

int ramdisk_tag_find(vfs_tag_t *this, const char *name, int name_len, vfs_tag_t *tag) {
  (void)this;
  (void)name;
  (void)name_len;
  (void)tag;
  return -1;
}

int ramdisk_tag_list(vfs_tag_t *this, vfs_iter_t *iter) {
  (void)this;
  (void)iter;
  return -1;
}

int ramdisk_tag_node(vfs_tag_t *this, vfs_node_t *node) {
  node->tag = *this;
  node->op = ramdisk_data.node_op;
  node->impl.data = 0;
  return 0;
}

int ramdisk_node_open(vfs_node_t *this, int flags, mode_t mode) {
  (void)this;
  (void)flags;
  (void)mode;
  return -1;
}

int ramdisk_node_close(vfs_node_t *this) {
  (void)this;
  return -1;
}

ssize_t ramdisk_node_read(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

ssize_t ramdisk_node_write(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

int ramdisk_node_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  return -1;
}

void* ramdisk_node_query(vfs_node_t *this, int iface_id) {
  switch (iface_id) {
  case VFS_IFACE_BDEV: return this->impl.data;
  default: return 0;
  }
}

int ramdisk_create(size_t size, vfs_tag_t *tag) {
  struct ramdisk_instance *instance = calloc(1, sizeof(*instance));
  // TODO: allocate virtual memory.
  (void)size;
  bdev_t *bdev = calloc(1, sizeof(*bdev));
  bdev->op = ramdisk_data.bdev_op;
  bdev->impl.data = instance;
  bdev->tag.op = ramdisk_data.tag_op;
  bdev->tag.impl.data = bdev;
  tag->op = ramdisk_data.tag_op;
  tag->impl.data = bdev;
  return 0;
}

void ramdisk_bind(ramdisk_t *ramdisk) {
  *ramdisk = ramdisk_data.ramdisk;
}

void ramdisk_init() {
  BROKER_REGISTER(RAMDISK_ID, REGISTER_SERVICE(ramdisk_bind));
  ramdisk_data.ramdisk.create = REGISTER_SERVICE(ramdisk_create);

  ramdisk_data.tag_op.find   = ramdisk_tag_find;
  ramdisk_data.tag_op.list   = ramdisk_tag_list;
  ramdisk_data.tag_op.node   = ramdisk_tag_node;

  ramdisk_data.node_op.open  = ramdisk_node_open;
  ramdisk_data.node_op.close = ramdisk_node_close;
  ramdisk_data.node_op.read  = ramdisk_node_read;
  ramdisk_data.node_op.write = ramdisk_node_write;
  ramdisk_data.node_op.stat  = ramdisk_node_stat;
  ramdisk_data.node_op.query = ramdisk_node_query;
}
