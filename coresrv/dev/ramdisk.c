#include <hellbender/dev/ramdisk.h>
#include <hellbender/dev/bdev.h>
#include <hellbender/fs/vfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>
#include <hellbender/sizes.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

static struct ramdisk {
  ramdisk_op_t ramdisk_op;
  struct vfs_node_op node_op;
  struct vfs_file_op file_op;
  struct bdev_op bdev_op;
} ramdisk;

struct ramdisk_instance {
  bdev_t bdev;
  uint8_t *buf;   // memory to hold size bytes.
  size_t size;
};

struct ramdisk_impl {
  struct ramdisk_instance *instance;
};

#define IMPL(n) ((struct ramdisk_impl*)&((n)->impl))

int ramdisk_open(vfs_node_t *this, int flags, vfs_file_t *file) {
  (void)this;
  (void)flags;
  (void)file;
  return -1;
}

int ramdisk_query(vfs_node_t *this, int iface_id, void **iface) {
  switch (iface_id) {
  case VFS_IFACE_BDEV:
    *iface = &IMPL(this)->instance->bdev;
    return 0;
  default: return ENOSYS;
  }
}

static int ramdisk_read(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  struct ramdisk_instance *instance = IMPL(this)->instance;
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  if ((size_t)offset >= instance->size) nbytes = 0;
  else if ((size_t)(nbytes + offset) > instance->size) nbytes = instance->size - offset;
  memcpy(buf, instance->buf + offset, nbytes);
  *nbytes_ = nbytes;
  offset_ += nbytes;
  return 0;
}

static int ramdisk_write(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  struct ramdisk_instance *instance = IMPL(this)->instance;
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  if ((size_t)offset >= instance->size) nbytes = 0;
  else if ((size_t)(nbytes + offset) > instance->size) nbytes = instance->size - offset;
  memcpy(instance->buf + offset, buf, nbytes);
  *nbytes_ = nbytes;
  offset_ += nbytes;
  return 0;
}

static int ramdisk_close(vfs_file_t *this) {
  (void)this;
  return -1;
}

int ramdisk_enqueue(vfs_file_t *this, vfs_io_t *io) {
  int errno;
  switch (io->opcode) {
  case VFS_OPCODE_CLOSE: 
    errno = ramdisk_close(this);
    break;
  case VFS_OPCODE_FLUSH: return 0;
  case VFS_OPCODE_READ: 
    errno = ramdisk_read(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_WRITE:
    errno = ramdisk_write(this, io->buf, &io->nbytes, &io->offset);
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

int ramdisk_cancel(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  (void)io;
  return VFS_CANCEL_ALLDONE;
}

int ramdisk_create(size_t size, vfs_node_t *node) {
  struct ramdisk_instance *instance = calloc(1, sizeof(*instance));
  instance->buf = (void*)syscall_alloc_tables((size + TABLE_SIZE - 1) / TABLE_SIZE);
  instance->size = size;
  instance->bdev.op = ramdisk.bdev_op;
  IMPL(&instance->bdev)->instance = instance;
  node->op = ramdisk.node_op;
  node->stat.st_size = size;
  node->stat.st_blksize = PAGE_SIZE;
  node->stat.st_blocks = (size + PAGE_SIZE - 1) / PAGE_SIZE;
  IMPL(node)->instance = instance;
  return 0;
}

void ramdisk_bind(ramdisk_op_t *ramdisk_op) {
  *ramdisk_op = ramdisk.ramdisk_op;
}

void ramdisk_init() {
  BROKER_REGISTER(RAMDISK_ID, REGISTER_SERVICE(ramdisk_bind));
  ramdisk.ramdisk_op.create = REGISTER_SERVICE(ramdisk_create);
  ramdisk.node_op.open      = REGISTER_SERVICE(ramdisk_open);
  ramdisk.node_op.query     = REGISTER_SERVICE(ramdisk_query);
  ramdisk.file_op.enqueue   = REGISTER_SERVICE(ramdisk_enqueue);
  ramdisk.file_op.cancel    = REGISTER_SERVICE(ramdisk_cancel);
}
