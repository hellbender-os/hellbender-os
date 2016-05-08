#include <hellbender/fs/devfs.h>
#include <hellbender/dev/bdev.h>
#include <hellbender/dev/cdev.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>
#include <hellbender/inline.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

static struct devfs {
  struct devfs_op devfs_op; 
  struct vfs_fs_op fs_op;
  struct vfs_node_op node_op;
  struct vfs_file_op file_op;
  dev_t next_dev;
} devfs;

struct devfs_instance {
  dev_t dev;
  ino_t next_ino;
  struct devfs_entry {
    struct devfs_entry *sibling;
    struct devfs_entry *children;
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
    vfs_node_t dev;
    dev_t rdev;
  } root;
};

struct devfs_impl {
  struct devfs_instance *instance;
  struct devfs_entry *node_entry;
  struct vfs_node *file_node;
};

#define IMPL(n) ((struct devfs_impl*)&((n)->impl))

static int devfs_stat(vfs_node_t *this, struct stat *buf) {
  struct devfs_instance *instance = IMPL(this)->instance;
  struct devfs_entry *entry = IMPL(this)->node_entry;
  buf->st_dev = instance->dev;
  buf->st_ino = entry->ino;
  buf->st_mode = entry->mode;
  buf->st_nlink = entry->nlink;
  buf->st_uid = entry->uid;
  buf->st_gid = entry->gid;
  buf->st_rdev = entry->rdev;
  buf->st_atime = entry->atime;
  buf->st_mtime = entry->mtime;
  buf->st_ctime = entry->ctime;
  if (entry->rdev) {
    buf->st_size = entry->dev.stat.st_size;
    buf->st_blksize = entry->dev.stat.st_blksize;
    buf->st_blocks = entry->dev.stat.st_blocks;
  } else {
    buf->st_size = (off_t)0;
    buf->st_blksize = (blksize_t)0;
    buf->st_blocks = (blkcnt_t)0;
  }
  return 0;
}

int devfs_root(vfs_fs_t *this, vfs_node_t *node) {
  node->op = devfs.node_op;
  node->impl = this->impl;
  IMPL(node)->node_entry = &(IMPL(this)->instance->root);
  devfs_stat(node, &node->stat);
  return 0;
}

int devfs_open(vfs_node_t *this, int flags, vfs_file_t *file) {
  struct devfs_entry *entry = IMPL(this)->node_entry;
  // TODO: update atime.
  // TODO: update mtime.
  if (!entry->rdev) {
    if (flags & O_RDONLY) {
      file->op = devfs.file_op;
      file->impl = this->impl;
      IMPL(file)->file_node = this;
      return 0;
    } else return EROFS;
  } else {
    return entry->dev.op.open(&entry->dev, flags, file);
  }
}

int devfs_query(vfs_node_t *this, int iface_id, void **iface) {
  struct devfs_entry *entry = IMPL(this)->node_entry;
  if (entry->rdev) {
    return entry->dev.op.query(&entry->dev, iface_id, iface);
  } else return ENOSYS;
}

static int devfs_readdir(vfs_node_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  struct devfs_entry *entry = IMPL(this)->node_entry;
  struct devfs_entry *child;
  if (offset == 0) child = entry->children;
  else child = (struct devfs_entry *)offset;
  size_t nread = 0;
  for (struct dirent *de = buf; child && nbytes >= sizeof(*de); 
       nbytes -= sizeof(*de), nread += sizeof(*de), child = entry->sibling) {
    de->d_ino = child->ino;
    memcpy(&de->d_name, child->name, child->name_len + 1);
    de->d_name_len = child->name_len;
  }
  *offset_ = (off_t)child;
  *nbytes_ = nread;
  return 0;
}

static int devfs_find(vfs_node_t *this, const char *name, int name_len, vfs_node_t *node) {
  struct devfs_entry *entry = IMPL(this)->node_entry;
  for (struct devfs_entry *child = entry->children; child; child = child->sibling) {
    if (strncmp(child->name, name, name_len) == 0
        && child->name[name_len] == '\0') {
      node->op = devfs.node_op;
      node->impl = this->impl;
      IMPL(node)->node_entry = child;
      devfs_stat(node, &node->stat);
      return 0;
    }
  }
  return -1;
}

int devfs_enqueue(vfs_file_t *this, vfs_io_t *io) {
  vfs_node_t *node = IMPL(this)->file_node;
  int errno;
  switch (io->opcode) {
  case VFS_OPCODE_CLOSE: return 0;
  case VFS_OPCODE_FLUSH: return 0;
  case VFS_OPCODE_READ: return ENOSYS;
  case VFS_OPCODE_WRITE: return EROFS;
  case VFS_OPCODE_READDIR:
    errno = devfs_readdir(node, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_MKNOD: return ENOSYS; //TODO implement
  case VFS_OPCODE_UNLINK: return ENOSYS; //TODO implement
  case VFS_OPCODE_FIND:
    errno = devfs_find(node, io->buf, io->nbytes, io->node);
    break;
  default: return EINVAL;
  }
  if (!errno) io->onsuccess(io); else io->onfailure(io, errno);
  return 0;
}

int devfs_cancel(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  (void)io;
  return VFS_CANCEL_ALLDONE;
}

dev_t devfs_alloc_dev() {
  return ++devfs.next_dev;
}

int devfs_create(vfs_fs_t *fs) {
  fs->op = devfs.fs_op;
  struct devfs_instance *instance = calloc(1, sizeof(*instance));
  instance->dev = devfs_alloc_dev();
  instance->root.ino = ++(instance->next_ino);
  instance->root.mode = (mode_t)(S_IFDIR | S_IRWXU);
  IMPL(fs)->instance = instance;
  return 0;
}

int devfs_add_dev(vfs_fs_t *fs, const char *name, vfs_node_t *node, dev_t *dev) {
  struct devfs_instance *instance = IMPL(fs)->instance;
  size_t name_len =  strlen(name);
  struct devfs_entry *entry = calloc(1, sizeof(*entry) + name_len + 1);
  char *name_copy = (char*)(entry + 1);
  strcpy(name_copy, name);
  entry->name = name_copy;
  entry->name_len = name_len;
  // TODO: set ctime
  entry->dev = *node;
  *dev = entry->rdev = devfs_alloc_dev();
  entry->sibling = instance->root.children;
  instance->root.children = entry;
  return 0;
}

void devfs_bind(devfs_op_t *devfs_op) {
  *devfs_op = devfs.devfs_op;
}

void devfs_init() {
  BROKER_REGISTER(DEVFS_ID, REGISTER_SERVICE(devfs_bind));
  devfs.devfs_op.create    = REGISTER_SERVICE(devfs_create);
  devfs.devfs_op.add_dev   = REGISTER_SERVICE(devfs_add_dev);
  devfs.devfs_op.alloc_dev = REGISTER_SERVICE(devfs_alloc_dev);
  devfs.node_op.open       = REGISTER_SERVICE(devfs_open);
  devfs.node_op.query      = REGISTER_SERVICE(devfs_query);
  devfs.file_op.enqueue    = REGISTER_SERVICE(devfs_enqueue);
  devfs.file_op.cancel     = REGISTER_SERVICE(devfs_cancel);
}
