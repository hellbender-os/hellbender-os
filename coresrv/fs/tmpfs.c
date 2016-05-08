#include <hellbender/fs/tmpfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>
#include <hellbender/inline.h>
#include <hellbender/sizes.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

static struct tmpfs {
  struct tmpfs_op tmpfs_op;
  struct vfs_fs_op fs_op;
  struct vfs_node_op node_op;
  struct vfs_file_op file_op;
} tmpfs;

struct tmpfs_instance {
  dev_t dev;
  ino_t next_ino;
  size_t size;
  size_t max_size; // TODO: enforce this limit.

  struct tmpfs_entry {
    struct tmpfs_entry *sibling;
    struct tmpfs_entry *children;
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

struct tmpfs_impl {
  struct tmpfs_instance *instance;
  struct tmpfs_entry *node_entry;
  struct vfs_node *file_node;
};

#define IMPL(n) ((struct tmpfs_impl*)&((n)->impl))

INLINE void* table_address(struct tmpfs_entry *entry, off_t offset) {
  if (offset < TABLE_SIZE) {
    return entry->direct_table;
  } else {
    if (!entry->indirect_table) {
      entry->indirect_table = (void*)syscall_alloc_tables(1);
      if (!entry->indirect_table) return 0;
    }
    unsigned idx = (offset - TABLE_SIZE) / TABLE_SIZE;
    void* ptr = entry->indirect_table[idx];
    if (!ptr) {
      ptr = entry->indirect_table[idx] = (void*)syscall_alloc_tables(1);
      if (!ptr) return 0;
    }
    return ptr;
  }
}

static int tmpfs_stat(vfs_node_t *this, struct stat *buf) {
  struct tmpfs_instance *instance = IMPL(this)->instance;
  struct tmpfs_entry *entry = IMPL(this)->node_entry;
  buf->st_dev = instance->dev;
  buf->st_ino = entry->ino;
  buf->st_mode = entry->mode;
  buf->st_nlink = entry->nlink;
  buf->st_uid = entry->uid;
  buf->st_gid = entry->gid;
  buf->st_rdev = 0;
  buf->st_size = (off_t)entry->filesize;
  buf->st_atime = entry->atime;
  buf->st_mtime = entry->mtime;
  buf->st_ctime = entry->ctime;
  buf->st_blksize = (blksize_t)TABLE_SIZE;
  buf->st_blocks = (blkcnt_t)(buf->st_size + TABLE_SIZE - 1) / TABLE_SIZE;
  return 0;
}

int tmpfs_root(vfs_fs_t *this, vfs_node_t *node) {
  node->op = tmpfs.node_op;
  node->impl = this->impl;
  IMPL(node)->node_entry = &IMPL(this)->instance->root;
  tmpfs_stat(node, &node->stat);
  return 0;
}

int tmpfs_open(vfs_node_t *this, int flags, vfs_file_t *file) {
  // TODO: update atime.
  // TODO: update mtime.
  file->op = tmpfs.file_op;
  file->impl = this->impl;
  file->oflags = flags;
  IMPL(file)->file_node = this;
  struct tmpfs_entry *entry = IMPL(this)->node_entry;
  if (flags & O_RDWR || flags & O_WRONLY) {
    if (entry->locked) return EACCES;
    entry->locked = 1;
  }
  return 0;
}

int tmpfs_query(vfs_node_t *this, int iface_id, void **iface) {
  (void)this;
  (void)iface_id;
  (void)iface;
  return ENOSYS;
}

static int tmpfs_read(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  struct tmpfs_entry *entry = IMPL(this)->node_entry;
  if ((size_t)offset >= entry->filesize) nbytes = 0;
  else if (offset + nbytes >= entry->filesize) nbytes = entry->filesize - offset;
  uint8_t *dst = buf;
  size_t nread = 0;
  while (nbytes) {
    uint8_t *base = table_address(entry, offset); // base address of the table
    if (!base) return EIO;
    unsigned idx = offset % TABLE_SIZE; // offset into the table.
    unsigned n = TABLE_SIZE - idx; // how many bytes to copy from this table.
    if (n > nbytes) n = nbytes;
    memcpy(dst, base + idx, n);
    dst += n;
    offset += n;
    nbytes -= n;
    nread += n;
  }
  *nbytes_ = nread;
  *offset_ += nread;
  return nread;
}

static int tmpfs_write(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  struct tmpfs_entry *entry = IMPL(this)->node_entry;
  uint8_t *src = buf;
  size_t nwritten = 0;
  while (nbytes) {
    uint8_t *base = table_address(entry, offset); // base address of the table
    if (!base) return EIO;
    unsigned idx = offset % TABLE_SIZE; // offset into the table.
    unsigned n = TABLE_SIZE - idx; // how many bytes to copy from this table.
    if (n > nbytes) n = nbytes;
    memcpy(base + idx, src, n);
    src += n;
    offset += n;
    nbytes -= n;
    nwritten += n;
  }
  *nbytes_ = nwritten;
  size_t newsize = (*offset_ += nwritten);
  if (newsize > entry->filesize) {
    IMPL(this)->instance->size += newsize - entry->filesize;
    entry->filesize = newsize;
  }
  return 0;
}

static int tmpfs_readdir(vfs_node_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  struct tmpfs_entry *entry = IMPL(this)->node_entry;
  struct tmpfs_entry *child;
  if (offset == 0) child = entry->children;
  else child = (struct tmpfs_entry *)offset;
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

static int tmpfs_find(vfs_node_t *this, const char *name, int name_len, vfs_node_t *node) {
  struct tmpfs_entry *entry = IMPL(this)->node_entry;
  for (struct tmpfs_entry *child = entry->children; child; child = child->sibling) {
    if (strncmp(child->name, name, name_len) == 0
        && child->name[name_len] == '\0') {
      node->op = tmpfs.node_op;
      node->impl = this->impl;
      IMPL(node)->node_entry = child;
      tmpfs_stat(node, &node->stat);
      return 0;
    }
  }
  return ENOENT;
}

static int tmpfs_close(vfs_file_t *this) {
  struct tmpfs_entry *entry = IMPL(this)->node_entry;
  if (this->oflags & O_RDWR || this->oflags & O_WRONLY) {
    entry->locked = 0;
  }
  return 0;
}

int tmpfs_enqueue(vfs_file_t *this, vfs_io_t *io) {
  vfs_node_t *node = IMPL(this)->file_node;
  int errno;
  switch (io->opcode) {
  case VFS_OPCODE_CLOSE: 
    errno = tmpfs_close(this);
    break;
  case VFS_OPCODE_FLUSH: return 0;
  case VFS_OPCODE_READ: 
    errno = tmpfs_read(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_WRITE:
    errno = tmpfs_write(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_READDIR:
    errno = tmpfs_readdir(node, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_MKNOD: return ENOSYS; //TODO implement
  case VFS_OPCODE_UNLINK: return ENOSYS; //TODO implement
  case VFS_OPCODE_FIND:
    errno = tmpfs_find(node, io->buf, io->nbytes, io->node);
    break;
  default: return EINVAL;
  }
  if (!errno) io->onsuccess(io); else io->onfailure(io, errno);
  return 0;
}

int tmpfs_cancel(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  (void)io;
  return VFS_CANCEL_ALLDONE;
}

int tmpfs_create(size_t max_size, dev_t dev, vfs_fs_t *fs) {
  fs->op = tmpfs.fs_op;
  struct tmpfs_instance *instance = calloc(1, sizeof(*instance));
  instance->dev = dev;
  instance->max_size = max_size;
  instance->root.ino = ++(instance->next_ino);
  instance->root.mode = (mode_t)(S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO);
  IMPL(fs)->instance = instance;
  return 0;
}

void tmpfs_bind(tmpfs_op_t *tmpfs_op) {
  *tmpfs_op = tmpfs.tmpfs_op;
}

void tmpfs_init() {
  BROKER_REGISTER(TMPFS_ID, REGISTER_SERVICE(tmpfs_bind));
  tmpfs.tmpfs_op.create = REGISTER_SERVICE(tmpfs_create);
  tmpfs.fs_op.root      = REGISTER_SERVICE(tmpfs_root);
  tmpfs.node_op.open    = REGISTER_SERVICE(tmpfs_open);
  tmpfs.node_op.query   = REGISTER_SERVICE(tmpfs_query);
  tmpfs.file_op.enqueue = REGISTER_SERVICE(tmpfs_enqueue);
  tmpfs.file_op.cancel  = REGISTER_SERVICE(tmpfs_cancel);
}
