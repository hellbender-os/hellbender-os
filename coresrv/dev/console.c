#include <hellbender/dev/console.h>
#include <hellbender/fs/vfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>

// cdev_t::impl.data == 0 // TODO: do we need multiple console instances?
// vfs_tag_t::impl.data == cdev_t
// vfs_node_t::impl.data == 0 // TODO: do we buffer for each opened console?

static struct console_data {
  console_t console;
  struct vfs_tag_op tag_op;
  struct vfs_node_op node_op;
  struct cdev_op cdev_op;
} console_data;

int console_tag_find(vfs_tag_t *this, const char *name, int name_len, vfs_tag_t *tag) {
  (void)this;
  (void)name;
  (void)name_len;
  (void)tag;
  return -1;
}

int console_tag_list(vfs_tag_t *this, vfs_iter_t *iter) {
  (void)this;
  (void)iter;
  return -1;
}

int console_tag_node(vfs_tag_t *this, vfs_node_t *node) {
  node->tag = *this;
  node->op = console_data.node_op;
  node->impl.data = 0;
  return 0;
}

int console_node_open(vfs_node_t *this, int flags, mode_t mode) {
  (void)this;
  (void)flags;
  (void)mode;
  return -1;
}

int console_node_close(vfs_node_t *this) {
  (void)this;
  return -1;
}

ssize_t console_node_read(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

ssize_t console_node_write(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

int console_node_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  return -1;
}

void* console_node_query(vfs_node_t *this, int iface_id) {
  switch (iface_id) {
  case VFS_IFACE_CDEV: return this->impl.data;
  default: return 0;
  }
}

int console_create(vfs_tag_t *tag) {
  cdev_t *cdev = calloc(1, sizeof(*cdev));
  cdev->op = console_data.cdev_op;
  cdev->tag.op = console_data.tag_op;
  cdev->tag.impl.data = cdev;
  *tag = cdev->tag;
  return 0;
}

void console_bind(console_t *console) {
  *console = console_data.console;
}

void console_init() {
  BROKER_REGISTER(CONSOLE_ID, REGISTER_SERVICE(console_bind));
  console_data.console.create = REGISTER_SERVICE(console_create);

  console_data.tag_op.find   = console_tag_find;
  console_data.tag_op.list   = console_tag_list;
  console_data.tag_op.node   = console_tag_node;

  console_data.node_op.open  = console_node_open;
  console_data.node_op.close = console_node_close;
  console_data.node_op.read  = console_node_read;
  console_data.node_op.write = console_node_write;
  console_data.node_op.stat  = console_node_stat;
  console_data.node_op.query = console_node_query;
}
