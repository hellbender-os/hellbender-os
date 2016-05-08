#include <hellbender/fs/fat16.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

static struct fat16 {
  struct fat16_op fat16_op;
  struct vfs_fs_op fs_op;
  struct vfs_node_op node_op;
  struct vfs_file_op file_op;
} fat16;

struct fat16_instance {
  vfs_node_t dev;
  ino_t next_ino;

  struct fat16_entry {
    struct fat16_entry *sibling;
    struct fat16_entry *children;
    int locked;
    ino_t ino;
    mode_t mode;
    uid_t uid;
    gid_t gid;
    uint16_t nlink;
    time_t atime;
    time_t mtime;
    time_t ctime;
    const char *name;
    size_t name_len;
    size_t filesize;
    uint8_t *direct_table;
    uint8_t **indirect_table;
  } root;
};

struct fat16_impl {
  struct fat16_instance *instance;
  struct fat16_entry *node_entry;
  struct vfs_node *file_node;
};

#define IMPL(n) ((struct fat16_impl*)&((n)->impl))

/*
static int fat16_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  return -1;
}
*/

int fat16_root(vfs_fs_t *this, vfs_node_t *node) {
  (void)this;
  (void)node;
  return -1;
}

int fat16_open(vfs_node_t *this, int flags, vfs_file_t *file) {
  (void)this;
  (void)flags;
  (void)file;
  return -1;
}

int fat16_query(vfs_node_t *this, int iface_id, void **iface) {
  (void)this;
  (void)iface_id;
  (void)iface;
  return ENOSYS;
}

/*
static int fat16_read(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  (void)this;
  (void)buf;
  (void)nbytes_;
  (void)offset_;
  return -1;
}
*/

/*
static int fat16_write(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  (void)this;
  (void)buf;
  (void)nbytes_;
  (void)offset_;
  return -1;
}
*/

/*
static int fat16_readdir(vfs_node_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  (void)this;
  (void)buf;
  (void)nbytes_;
  (void)offset_;
  return -1;
}
*/

/*
static int fat16_find(vfs_node_t *this, const char *name, int name_len, vfs_node_t *node) {
  (void)this;
  (void)name;
  (void)name_len;
  (void)node;
  return -1;
}
*/

/*
int fat16_close(vfs_file_t *this) {
  (void)this;
  return -1;
}
*/

int fat16_enqueue(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  //vfs_node_t *node = IMPL(this)->file_node;
  int errno;
  switch (io->opcode) {
  case VFS_OPCODE_CLOSE: return -1;
  case VFS_OPCODE_FLUSH: return -1;
  case VFS_OPCODE_READ: return -1;
  case VFS_OPCODE_WRITE: return -1;
  case VFS_OPCODE_READDIR: return -1;
  case VFS_OPCODE_MKNOD: return -1;
  case VFS_OPCODE_UNLINK: return -1;
  case VFS_OPCODE_FIND: return -1;
  default: return EINVAL;
  }
  if (!errno) io->onsuccess(io); else io->onfailure(io, errno);
  return 0;
}

int fat16_cancel(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  (void)io;
  return VFS_CANCEL_ALLDONE;
}

int fat16_create(vfs_node_t *dev, vfs_fs_t *fs) {
  struct fat16_instance *instance = calloc(1, sizeof(*instance));
  instance->dev = *dev;
  IMPL(fs)->instance = instance;
  return 0;
}

void fat16_bind(fat16_op_t *fat16_op) {
  *fat16_op = fat16.fat16_op;
}

void fat16_init() {
  BROKER_REGISTER(FAT16_ID, REGISTER_SERVICE(fat16_bind));
  fat16.fat16_op.create = REGISTER_SERVICE(fat16_create);
  fat16.fs_op.root      = REGISTER_SERVICE(fat16_root);
  fat16.node_op.open    = REGISTER_SERVICE(fat16_open);
  fat16.node_op.query   = REGISTER_SERVICE(fat16_query);
  fat16.file_op.enqueue = REGISTER_SERVICE(fat16_enqueue);
  fat16.file_op.cancel  = REGISTER_SERVICE(fat16_cancel);
}
