#include <hellbender/fs/initfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>
#include <string.h>

static struct initfs_data {
  initfs_t initfs;
  struct vfs_tag_op tag_op;
  struct vfs_node_op node_op;
} initfs_data;

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
  uint8_t *buffer;
  size_t size;

  struct initfs_entry {
    struct initfs_instance *instance;
    struct initfs_entry *next;
    struct initfs_entry *children;
    struct initfs_entry *flat_next;
    struct initfs_header *header;
    const char *name;
    size_t name_len;
    const char *path;
    const uint8_t *data;
    size_t filesize;
  } root, *flat_first;
};

static void set_tag(struct initfs_entry *e, vfs_tag_t *t) {
  struct initfs_header *h = e->header;
  t->op = initfs_data.tag_op;
  t->ino = h->ino;
  t->uid = h->uid;
  t->gid = h->gid;
  t->mode = h->mode;
  t->impl.data = e;
}

int initfs_find(vfs_tag_t *this, const char *name, int name_len, vfs_tag_t *tag) {
  struct initfs_entry *entry = this->impl.data;
  for (struct initfs_entry *child = entry->children; child; child = child->next) {
    if (strncmp(child->name, name, name_len) == 0
        && child->name[name_len] == '\0') {
      set_tag(child, tag);
      return 0;
    }
  }
  return -1;
}

static int set_iter(struct initfs_entry *e, vfs_iter_t *i) {
  if (e) {
    set_tag(e, &i->tag);
    strcpy(i->name, e->name);
    i->impl.data = e;
    return 0;
  } else return -1;
}

int initfs_next(vfs_iter_t *this) {
  struct initfs_entry *entry = this->impl.data;
  struct initfs_entry *child = entry->next;
  return set_iter(child, this);
}

int initfs_list(vfs_tag_t *this, vfs_iter_t *iter) {
  struct initfs_entry *entry = this->impl.data;
  struct initfs_entry *child = entry->children;
  iter->next = initfs_next;
  return set_iter(child, iter);
}

int initfs_node(vfs_tag_t *this, vfs_node_t *node) {
  node->tag = *this;
  node->op = initfs_data.node_op;
  node->impl.data = 0;
  return 0;
}

int initfs_open(vfs_node_t *this, int flags, mode_t mode) {
  (void)this;
  (void)flags;
  (void)mode;
  return -1;
}

int initfs_close(vfs_node_t *this) {
  (void)this;
  return -1;
}

ssize_t initfs_read(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

ssize_t initfs_write(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

int initfs_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  return -1;
}

void* initfs_query(vfs_node_t *this, int iface_id) {
  (void)this;
  (void)iface_id;
  return 0;
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
    if (parent) {
      entry->next = parent->children;
      parent->children = entry;
    }
  }
  return has_root ? 0 : -1;
}

int initfs_create(void *initrd_base, size_t initrd_size, vfs_tag_t *root) {
  struct initfs_instance *instance = calloc(1, sizeof(*instance));
  instance->buffer = (uint8_t*)initrd_base;
  instance->size = initrd_size;
  if (!initfs_parse(instance)) {
    set_tag(&instance->root, root);
    return 0;
  } else return 1;
}

void initfs_bind(initfs_t *initfs) {
  *initfs = initfs_data.initfs;
}

void initfs_init() {
  BROKER_REGISTER(INITFS_ID, REGISTER_SERVICE(initfs_bind));
  initfs_data.initfs.create = REGISTER_SERVICE(initfs_create);

  initfs_data.tag_op.find = initfs_find;
  initfs_data.tag_op.list = initfs_list;
  initfs_data.tag_op.node = initfs_node;
  
  initfs_data.node_op.open  = initfs_open;
  initfs_data.node_op.close = initfs_close;
  initfs_data.node_op.read  = initfs_read;
  initfs_data.node_op.write = initfs_write;
  initfs_data.node_op.stat  = initfs_stat;
  initfs_data.node_op.query = initfs_query;
}
