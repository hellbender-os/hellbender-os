#include <hellbender/fs/tmpfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>
#include <string.h>

static struct tmpfs_data {
  tmpfs_t tmpfs; 
  struct vfs_tag_op tag_op;
  struct vfs_node_op node_op;
} tmpfs_data;

struct tmpfs_instance {
  size_t size;
  size_t max_size;

  struct tmpfs_entry {
    struct tmpfs_instance *instance;
    struct tmpfs_entry *next;
    struct tmpfs_entry *children;
    uint32_t ino;
    uint16_t mode;
    uint16_t uid;
    uint16_t gid;
    uint16_t nlink;
    uint32_t mtime;
    const char *name;
    size_t name_len;
    const uint8_t *data;
    size_t filesize;
  } root;
};

static void set_tag(struct tmpfs_entry *e, vfs_tag_t *t) {
  t->op = tmpfs_data.tag_op;
  t->ino = e->ino;
  t->uid = e->uid;
  t->gid = e->gid;
  t->mode = e->mode;
  t->impl.data = e;
}

int tmpfs_find(vfs_tag_t *this, const char *name, int name_len, vfs_tag_t *tag) {
  struct tmpfs_entry *entry = this->impl.data;
  for (struct tmpfs_entry *child = entry->children; child; child = child->next) {
    if (strncmp(child->name, name, name_len) == 0
        && child->name[name_len] == '\0') {
      set_tag(child, tag);
      return 0;
    }
  }
  return -1;
}

static int set_iter(struct tmpfs_entry *e, vfs_iter_t *i) {
  if (e) {
    set_tag(e, &i->tag);
    strcpy(i->name, e->name);
    i->impl.data = e;
    return 0;
  } else return -1;
}

int tmpfs_next(vfs_iter_t *this) {
  struct tmpfs_entry *entry = this->impl.data;
  struct tmpfs_entry *child = entry->next;
  return set_iter(child, this);
}

int tmpfs_list(vfs_tag_t *this, vfs_iter_t *iter) {
  struct tmpfs_entry *entry = this->impl.data;
  struct tmpfs_entry *child = entry->children;
  iter->next = tmpfs_next;
  return set_iter(child, iter);
}

int tmpfs_node(vfs_tag_t *this, vfs_node_t *node) {
  struct tmpfs_entry *entry = this->impl.data;
  set_tag(entry, &node->tag);
  node->op = tmpfs_data.node_op;
  node->impl.data = entry;
  return 0;
}

int tmpfs_open(vfs_node_t *this, int flags, mode_t mode) {
  (void)this;
  (void)flags;
  (void)mode;
  return -1;
}

int tmpfs_close(vfs_node_t *this) {
  (void)this;
  return -1;
}

ssize_t tmpfs_read(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

ssize_t tmpfs_write(vfs_node_t *this, void *buf, size_t nbyte, off_t offset) {
  (void)this;
  (void)buf;
  (void)nbyte;
  (void)offset;
  return -1;
}

int tmpfs_stat(vfs_node_t *this, struct stat *buf) {
  (void)this;
  (void)buf;
  return -1;
}

void* tmpfs_query(vfs_node_t *this, int iface_id) {
  (void)this;
  (void)iface_id;
  return 0;
}

int tmpfs_create(size_t max_size, vfs_tag_t *root) {
  struct tmpfs_instance *instance = calloc(1, sizeof(*instance));
  instance->max_size = max_size;
  // TODO: create root directory.
  set_tag(&instance->root, root);
  return 0;
}

void tmpfs_bind(tmpfs_t *tmpfs) {
  *tmpfs = tmpfs_data.tmpfs;
}

void tmpfs_init() {
  BROKER_REGISTER(TMPFS_ID, REGISTER_SERVICE(tmpfs_bind));
  tmpfs_data.tmpfs.create = REGISTER_SERVICE(tmpfs_create);

  tmpfs_data.tag_op.find = tmpfs_find;
  tmpfs_data.tag_op.list = tmpfs_list;
  tmpfs_data.tag_op.node = tmpfs_node;
  
  tmpfs_data.node_op.open  = tmpfs_open;
  tmpfs_data.node_op.close = tmpfs_close;
  tmpfs_data.node_op.read  = tmpfs_read;
  tmpfs_data.node_op.write = tmpfs_write;
  tmpfs_data.node_op.stat  = tmpfs_stat;
  tmpfs_data.node_op.query = tmpfs_query;
}
