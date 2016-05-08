#include <hellbender/fs/initfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>
#include <hellbender/inline.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

static struct initfs {
  struct initfs_op initfs_op;
  struct vfs_fs_op fs_op;
  struct vfs_node_op node_op;
  struct vfs_file_op file_op;
} initfs;

struct initfs_header {
  uint16_t magic;
  uint16_t dev;
  uint16_t ino;
  uint16_t mode;
  uint16_t uid;
  uint16_t gid;
  uint16_t nlink;
  uint16_t rdev;
  uint16_t mtime_hi;
  uint16_t mtime_lo;
  uint16_t namesize;
  uint16_t filesize_hi;
  uint16_t filesize_lo;
} __attribute__((packed));

struct initfs_instance {
  dev_t dev;
  uint8_t *buffer;
  size_t size;

  struct initfs_entry {
    struct initfs_entry *sibling;
    struct initfs_entry *children;
    struct initfs_entry *flat_next;
    struct initfs_header *header;
    const char *name;
    size_t name_len;
    const char *path;
    const uint8_t *data;
    size_t filesize;
    time_t atime; // last access time.
  } root, *flat_first;
};

struct initfs_impl {
  struct initfs_instance *instance;
  struct initfs_entry *node_entry;
  struct vfs_node *file_node;
};

#define IMPL(n) ((struct initfs_impl*)&((n)->impl))

static int initfs_stat(vfs_node_t *this, struct stat *buf) {
  struct initfs_instance *instance = IMPL(this)->instance;
  struct initfs_entry *entry = IMPL(this)->node_entry;
  struct initfs_header *header = entry->header;
  buf->st_dev = instance->dev;
  buf->st_ino = (ino_t)header->ino;
  buf->st_mode = (mode_t)header->mode;
  buf->st_nlink = (nlink_t)1;
  buf->st_uid = (uid_t)header->uid;
  buf->st_gid = (gid_t)header->gid;
  buf->st_rdev = (dev_t)header->rdev;
  buf->st_size = (off_t)entry->filesize;
  buf->st_atime = (time_t)entry->atime;
  buf->st_mtime = (time_t)header->mtime_lo + (((time_t)header->mtime_hi)<<16);
  buf->st_ctime = buf->st_mtime;
  buf->st_blksize = (blksize_t)buf->st_size;
  buf->st_blocks = (blkcnt_t)1;
  return 0;
}

int initfs_root(vfs_fs_t *this, vfs_node_t *node) {
  node->op = initfs.node_op;
  node->impl = this->impl;
  IMPL(node)->node_entry = &IMPL(this)->instance->root;
  initfs_stat(node, &node->stat);
  return 0;
}

int initfs_open(vfs_node_t *this, int flags, vfs_file_t *file) {
  // TODO: update atime.
  file->op = initfs.file_op;
  file->impl = this->impl;
  IMPL(file)->file_node = this;
  if (flags & O_RDONLY) {
    return 0;
  } else return EROFS;
}

int initfs_query(vfs_node_t *this, int iface_id, void **iface) {
  (void)this;
  (void)iface_id;
  (void)iface;
  return ENOSYS;
}

static int initfs_read(vfs_file_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  struct initfs_entry *entry = IMPL(this)->node_entry;
  if ((size_t)offset >= entry->filesize) nbytes = 0;
  else if ((offset + nbytes) >= entry->filesize) nbytes = entry->filesize - offset;
  memcpy(buf, entry->data, nbytes);
  *nbytes_ = nbytes;
  *offset_ += nbytes;
  return 0;
}

static int initfs_readdir(vfs_node_t *this, void *buf, size_t *nbytes_, off_t *offset_) {
  size_t nbytes = *nbytes_;
  off_t offset = *offset_;
  struct initfs_entry *entry = IMPL(this)->node_entry;
  struct initfs_entry *child;
  if (offset == 0) child = entry->children;
  else child = (struct initfs_entry *)offset;
  size_t nread = 0;
  for (struct dirent *de = buf; child && nbytes >= sizeof(*de); 
       nbytes -= sizeof(*de), nread += sizeof(*de), child = entry->sibling) {
    de->d_ino = child->header->ino;
    memcpy(&de->d_name, child->name, child->name_len + 1);
    de->d_name_len = child->name_len;
  }
  *offset_ = (off_t)child;
  *nbytes_ = nread;
  return 0;
}

static int initfs_find(vfs_node_t *this, const char *name, int name_len, vfs_node_t *node) {
  struct initfs_entry *entry = IMPL(this)->node_entry;
  for (struct initfs_entry *child = entry->children; child; child = child->sibling) {
    if (strncmp(child->name, name, name_len) == 0
        && child->name[name_len] == '\0') {
      node->op = initfs.node_op;
      node->impl = this->impl;
      IMPL(node)->node_entry = child;
      initfs_stat(node, &node->stat);
      return 0;
    }
  }
  return ENOENT;
}

int initfs_enqueue(vfs_file_t *this, vfs_io_t *io) {
  vfs_node_t *node = IMPL(this)->file_node;
  int errno;
  switch (io->opcode) {
  case VFS_OPCODE_CLOSE: return 0;
  case VFS_OPCODE_FLUSH: return 0;
  case VFS_OPCODE_READ: 
    errno = initfs_read(this, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_WRITE: return EROFS;
  case VFS_OPCODE_READDIR:
    errno = initfs_readdir(node, io->buf, &io->nbytes, &io->offset);
    break;
  case VFS_OPCODE_MKNOD: return EROFS;
  case VFS_OPCODE_UNLINK: return EROFS;
  case VFS_OPCODE_FIND:
    errno = initfs_find(node, io->buf, io->nbytes, io->node);
    break;
  default: return EINVAL;
  }
  if (!errno) io->onsuccess(io); else io->onfailure(io, errno);
  return 0;
}

int initfs_cancel(vfs_file_t *this, vfs_io_t *io) {
  (void)this;
  (void)io;
  return VFS_CANCEL_ALLDONE;
}

// like strrchr but skips the last character (dir names end with separator).
static char *strrchr2(const char *str, int ch) {
  char *ptr = NULL;
  for (; *str && *(str+1); ++str) if (*str == ch) ptr = (char*)str;
  return ptr;
}

static struct initfs_entry* find_parent(const char *path, struct initfs_entry *ptr) {
  char *path_end = strrchr2(path, '/');
  if (!path_end) return NULL;
  size_t path_len = path_end - path + 1;
  for (; ptr; ptr = ptr->flat_next) {
    if (strncmp(ptr->path, path, path_len) == 0
        && strlen(ptr->path) == path_len) {
      return ptr;
    }
  }
  return 0;
}

static int initfs_parse(struct initfs_instance *instance) {
  // scan the buffer, build a directory hierarchy.
  uint8_t *buffer = instance->buffer;
  size_t size = instance->size;
  int has_root = 0;
  instance->root.flat_next = 0;
  while (size > sizeof(struct initfs_header)) {
    struct initfs_header *header = (struct initfs_header*)buffer;
    if (header->magic != 070707) {
      return -1;
    }
    size_t filesize = ((unsigned)header->filesize_lo
                       + (((unsigned)header->filesize_hi)<<16));
    //printf("dev=%u, ino=%u, mode=%u, uid=%u, gid=%u, nlink=%u, "
    //       "rdev=%u, mtime=%u, namesize=%u, filesize=%u %u\n",
    //       header->dev, header->ino, header->mode, header->uid,
    //       header->gid, header->nlink, header->rdev,
    //       (unsigned)header->mtime_lo + (((unsigned)header->mtime_hi)<<16),
    //       header->namesize, header->filesize_hi, header->filesize_lo);
    size -= sizeof(struct initfs_header);
    buffer += sizeof(struct initfs_header);
    char *fullpath = (char*)buffer;
    if (filesize == 0
        && header->mode == 0
        && strncmp(fullpath, "TRAILER!!", 9) == 0) break;
    buffer += header->namesize + (header->namesize&1);
    uint8_t *data = (uint8_t*)buffer;
    buffer += filesize + (filesize&1);
    struct initfs_entry *entry = NULL;
    struct initfs_entry *parent = NULL;
    if (strcmp(fullpath, "./") == 0) {
      entry = &instance->root;
      has_root = 1;
    } else {
      parent = find_parent(fullpath, instance->root.flat_next);
      if (!parent) parent = &instance->root;
      entry = calloc(1, sizeof(*entry));
      entry->flat_next = instance->root.flat_next;
      instance->root.flat_next = entry;
    }
    entry->header = header;
    char *path_end = strrchr2(fullpath, '/');
    entry->name = path_end ? path_end + 1 : fullpath;
    entry->name_len = fullpath + header->namesize - entry->name - 1;
    if (entry->name_len > NAME_MAX) return -1;
    entry->path = fullpath;
    entry->data = filesize ? data : NULL;
    entry->filesize = filesize;
    entry->atime = (time_t)header->mtime_lo + (((time_t)header->mtime_hi)<<16);
    if (parent) {
      entry->sibling = parent->children;
      parent->children = entry;
    }
  }
  return has_root ? 0 : -1;
}

int initfs_create(void *initrd_base, size_t initrd_size, dev_t dev, vfs_fs_t *fs) {
  fs->op = initfs.fs_op;
  struct initfs_instance *instance = calloc(1, sizeof(*instance));
  instance->dev = dev;
  instance->buffer = (uint8_t*)initrd_base;
  instance->size = initrd_size;
  if (!initfs_parse(instance)) {
    IMPL(fs)->instance = instance;
    return 0;
  } else {
    free(instance);
    return -1;
  }
}

void initfs_bind(initfs_op_t *initfs_op) {
  *initfs_op = initfs.initfs_op;
}

void initfs_init() {
  BROKER_REGISTER(INITFS_ID, REGISTER_SERVICE(initfs_bind));
  initfs.initfs_op.create = REGISTER_SERVICE(initfs_create);
  initfs.fs_op.root       = REGISTER_SERVICE(initfs_root);
  initfs.node_op.open     = REGISTER_SERVICE(initfs_open);
  initfs.node_op.query    = REGISTER_SERVICE(initfs_query);
  initfs.file_op.enqueue  = REGISTER_SERVICE(initfs_enqueue);
  initfs.file_op.cancel   = REGISTER_SERVICE(initfs_cancel);
}
