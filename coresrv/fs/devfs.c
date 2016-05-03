#include <hellbender/fs/devfs.h>
#include <hellbender/dev/bdev.h>
#include <hellbender/dev/cdev.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

#include <stdlib.h>
#include <string.h>

// root:
// vfs_tag_t::impl.data == struct devfs_instance
// vfs_iter_t::impl.data == struct devfs_entry

// root children are device nodes.

static struct devfs_data {
  devfs_t devfs; 
  struct vfs_tag_op tag_op;
} devfs_data;

struct devfs_instance {
  struct devfs_entry {
    struct devfs_entry *next;
    const char *name;
    size_t name_len;
    vfs_tag_t dev;
  } *children;
};

int devfs_find(vfs_tag_t *this, const char *name, int name_len, vfs_tag_t *tag) {
  struct devfs_instance *instance = this->impl.data;
  for (struct devfs_entry *child = instance->children; child; child = child->next) {
    if (strncmp(child->name, name, name_len) == 0
        && child->name[name_len] == '\0') {
      *tag = child->dev;
      return 0;
    }
  }
  return -1;
}

static int set_iter(struct devfs_entry *e, vfs_iter_t *i) {
  if (e) {
    i->tag = e->dev;
    strcpy(i->name, e->name);
    i->impl.data = e;
    return 0;
  } else return -1;
}

int devfs_next(vfs_iter_t *this) {
  struct devfs_entry *entry = this->impl.data;
  struct devfs_entry *child = entry->next;
  return set_iter(child, this);
}

int devfs_list(vfs_tag_t *this, vfs_iter_t *iter) {
  struct devfs_instance *instance = this->impl.data;
  struct devfs_entry *child = instance->children;
  iter->next = devfs_next;
  return set_iter(child, iter);
}

int devfs_node(vfs_tag_t *this, vfs_node_t *node) {
  node->tag = *this;
  // TODO: node op support?
  node->op = (struct vfs_node_op){0};
  node->impl.data = 0;
  return 0;
}

int devfs_create(vfs_tag_t *root) {
  struct devfs_instance *instance = calloc(1, sizeof(*instance));
  root->op = devfs_data.tag_op;
  root->impl.data = instance;
  return 0;
}

int devfs_add_dev(vfs_tag_t *root, const char *name, vfs_tag_t *tag) {
  struct devfs_instance *instance = root->impl.data;
  size_t name_len =  strlen(name);
  struct devfs_entry *entry = calloc(1, sizeof(*entry) + name_len + 1);
  char *name_copy = (char*)(entry + 1);
  strcpy(name_copy, name);
  entry->name = name_copy;
  entry->name_len = name_len;
  entry->dev = *tag;
  entry->next = instance->children;
  instance->children = entry;
  return 0;
}

void devfs_bind(devfs_t *devfs) {
  *devfs = devfs_data.devfs;
}

void devfs_init() {
  BROKER_REGISTER(DEVFS_ID, REGISTER_SERVICE(devfs_bind));
  devfs_data.devfs.create  = REGISTER_SERVICE(devfs_create);
  devfs_data.devfs.add_dev = REGISTER_SERVICE(devfs_add_dev);

  devfs_data.tag_op.find = devfs_find;
  devfs_data.tag_op.list = devfs_list;
  devfs_data.tag_op.node = devfs_node;
}
