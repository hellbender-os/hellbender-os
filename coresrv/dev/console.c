#include <hellbender/dev/console.h>
#include <hellbender/dev/cdev.h>
#include <hellbender/fs/vfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>
#include <errno.h>

static struct console {
  struct console_op console_op;
  struct vfs_node_op node_op;
  struct vfs_file_op file_op;
  struct cdev_op cdev_op;
} console;

struct console_instance {
  cdev_t cdev;
};

struct console_impl {
  struct console_instance *instance;
};

#define IMPL(n) ((struct console_impl*)&((n)->impl))

int console_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  return -1;
}

int console_open(vfs_node_t *this, int flags, vfs_file_t *file) {
  (void)this;
  (void)flags;
  (void)file;
  return -1;
}

int console_query(vfs_node_t *this, int iface_id, void **iface) {
  switch (iface_id) {
  case VFS_IFACE_CDEV:
    *iface = &IMPL(this)->instance->cdev;
    return 0;
  default: return ENOSYS;
  }
}

static int console_read(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  (void)this;
  (void)buf;
  (void)nbytes_;
  (void)offset_;
  return -1;
}

static int console_write(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  (void)this;
  (void)buf;
  (void)nbytes_;
  (void)offset_;
  return -1;
}

static int console_close(vfs_file_t *this) {
  (void)this;
  return -1;
}

int console_enqueue(vfs_file_t *this, vfs_io_t *io) {
  int errno;
  switch (io->opcode) {
  case VFS_OPCODE_CLOSE: 
    errno = console_close(this);
    break;
  case VFS_OPCODE_FLUSH: 
    // TODO: write buffer to screen?
    return 0;
  case VFS_OPCODE_READ: 
    errno = console_read(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_WRITE:
    errno = console_write(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_READDIR: return ENOSYS;
  case VFS_OPCODE_MKNOD: return ENOSYS;
  case VFS_OPCODE_UNLINK: return ENOSYS;
  case VFS_OPCODE_FIND: return ENOSYS;
  default: return EINVAL;
  }
  if (!errno) io->onsuccess(io); else io->onfailure(io, errno);
  return 0;
}

int console_cancel(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  (void)io;
  return VFS_CANCEL_ALLDONE;
}

int console_create(vfs_node_t *node) {
  struct console_instance *instance = calloc(1, sizeof(*instance));
  instance->cdev.op = console.cdev_op;
  IMPL(&instance->cdev)->instance = instance;
  node->op = console.node_op;
  node->stat.st_size = 0;
  node->stat.st_blksize = 1;
  node->stat.st_blocks = 0;
  IMPL(node)->instance = instance;
  return 0;
}

void console_bind(console_op_t *console_op) {
  *console_op = console.console_op;
}

void console_init() {
  BROKER_REGISTER(CONSOLE_ID, REGISTER_SERVICE(console_bind));
  console.console_op.create = REGISTER_SERVICE(console_create);
  console.node_op.open      = REGISTER_SERVICE(console_open);
  console.node_op.query     = REGISTER_SERVICE(console_query);
  console.file_op.enqueue   = REGISTER_SERVICE(console_enqueue);
  console.file_op.cancel    = REGISTER_SERVICE(console_cancel);
}
