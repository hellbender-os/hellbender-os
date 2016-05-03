#include <hellbender/fs/fat16.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>

static struct fat16_data {
  fat16_t fat16;
  struct vfs_tag_op tag_op;
  struct vfs_node_op node_op;
} fat16_data;

struct fat16_instance {
  vfs_tag_t dev;
};

int fat16_find(vfs_tag_t *this, const char *name, int name_len, vfs_tag_t *tag) {
  (void)this;
  (void)name;
  (void)name_len;
  (void)tag;
  return -1;
}

int fat16_next(vfs_iter_t *this) {
  (void)this;
  return -1;
}

int fat16_list(vfs_tag_t *this, vfs_iter_t *iter) {
  (void)this;
  (void)iter;
  return -1;
}

int fat16_node(vfs_tag_t *this, vfs_node_t *node) {
  (void)this;
  (void)node;
  return -1;
}

int fat16_open(vfs_node_t *this, int flags, mode_t mode) {
  (void)this;
  (void)flags;
  (void)mode;
  return -1;
}

int fat16_close(vfs_node_t *this) {
  (void)this;
  return -1;
}

ssize_t fat16_read(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

ssize_t fat16_write(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

int fat16_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  return -1;
}

void* fat16_query(vfs_node_t *this, int iface_id) {
  (void)this;
  (void)iface_id;
  return 0;
}

int fat16_create(vfs_tag_t *dev, vfs_tag_t *root) {
  struct fat16_instance *instance = calloc(1, sizeof(*instance));
  instance->dev = *dev;
  root->impl.data = instance;
  return 0;
}

void fat16_bind(fat16_t *fat16) {
  *fat16 = fat16_data.fat16;
}

void fat16_init() {
  BROKER_REGISTER(FAT16_ID, REGISTER_SERVICE(fat16_bind));
  fat16_data.fat16.create = REGISTER_SERVICE(fat16_create);

  fat16_data.tag_op.find = fat16_find;
  fat16_data.tag_op.list = fat16_list;
  fat16_data.tag_op.node = fat16_node;
  
  fat16_data.node_op.open  = fat16_open;
  fat16_data.node_op.close = fat16_close;
  fat16_data.node_op.read  = fat16_read;
  fat16_data.node_op.write = fat16_write;
  fat16_data.node_op.stat  = fat16_stat;
  fat16_data.node_op.query = fat16_query;
}
