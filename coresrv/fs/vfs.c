#include <hellbender/fs/vfs.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>
#include <hellbender/dict.h>
#include <hellbender/guard.h>

#include <string.h>
#include <stdlib.h>
#include <pthread.h>

static struct vfs_data {
  vfs_t vfs;
  dict_t mounts;
  pthread_rwlock_t rwlock;
} vfs_data;

struct vfs_item {
  dict_item_t item;
  vfs_node_t root;
}; // followed by mount path

int vfs_mount(const char *path, vfs_tag_t *root) {
  GUARD_PTHREAD_RWLOCK_WRITE(&vfs_data.rwlock);

  int key_len = strlen(path);
  if (key_len == 0) return -1;
  struct vfs_item *vi = (struct vfs_item*)malloc(sizeof(struct vfs_item) + key_len + 1);
  char *key = ((char *)vi) + sizeof(struct vfs_item);
  strcpy(key, path);
  vi->item.key = key;
  vi->item.key_len = key_len;
  if (!dict_insert(&vfs_data.mounts, &vi->item)) {
    root->op.node(root, &vi->root);
    return 0;
  } else return -1;
}

int vfs_unmount(const char *path, vfs_node_t *root) {
  GUARD_PTHREAD_RWLOCK_WRITE(&vfs_data.rwlock);

  dict_item_t *ptr = dict_find(&vfs_data.mounts, path, strlen(path));
  if (ptr) {
    struct vfs_item *vi = dict_container(ptr, struct vfs_item, item);
    dict_erase(&vfs_data.mounts, ptr);
    vi->root.tag.op.node(&vi->root.tag, root);
    free(vi);
    return 0;
  } else return -1;
}

int vfs_traverse(const char *path, vfs_node_t *node) {
  GUARD_PTHREAD_RWLOCK_READ(&vfs_data.rwlock);

  vfs_tag_t tag1, tag2;
  vfs_tag_t *tag_ptr = 0;
  const char *prev = path;
  const char *curr = strchr(prev, '/');
  size_t len = curr ? (curr == prev ? 1 : (size_t)(curr - path)) : strlen(path);
  while (*prev) {
    // TODO: if tag_ptr is symbolic link, traverse the symbol path first.
    dict_item_t *ptr = dict_find(&vfs_data.mounts, path, len);
    if (ptr) { // mount point found.
      struct vfs_item *item = dict_container(ptr, struct vfs_item, item);
      tag_ptr = &item->root.tag;
    } else if (!tag_ptr) { break;
    } else if (prev != curr) {
      vfs_tag_t *next = tag_ptr == &tag1 ? &tag2 : &tag1;
      tag_ptr->op.find(tag_ptr, prev, curr - prev, next);
      tag_ptr = next;
    }
    prev = curr + 1;
    curr = strchr(prev, '/');
    if (!curr) curr = prev + strlen(prev);
    len = curr - path;
  }

  if (tag_ptr) {
    tag_ptr->op.node(tag_ptr, node);
    return 0;
  } else return -1;
}

void vfs_bind(vfs_t *vfs) {
  *vfs = vfs_data.vfs;
}

void vfs_init() {
  vfs_data.rwlock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
  dict_init(&vfs_data.mounts);

  BROKER_REGISTER(VFS_ID, REGISTER_SERVICE(vfs_bind));
  vfs_data.vfs.mount = REGISTER_SERVICE(vfs_mount);
  vfs_data.vfs.unmount = REGISTER_SERVICE(vfs_unmount);
  vfs_data.vfs.traverse = REGISTER_SERVICE(vfs_traverse);
}
