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
  vfs_fs_t fs;
}; // followed by mount path

int vfs_mount(const char *path, vfs_fs_t *fs) {
  GUARD_PTHREAD_RWLOCK_WRITE(&vfs_data.rwlock);

  int key_len = strlen(path);
  if (key_len == 0) return -1;
  struct vfs_item *vi = malloc(sizeof(*vi) + key_len + 1);
  char *key = ((char *)vi) + sizeof(*vi);
  strcpy(key, path); 
  vi->fs = *fs;
  vi->item.key = key;
  vi->item.key_len = key_len;
  if (!dict_insert(&vfs_data.mounts, &vi->item)) {
    return 0;
  } else return -1;
}

int vfs_unmount(const char *path, vfs_fs_t *fs) {
  GUARD_PTHREAD_RWLOCK_WRITE(&vfs_data.rwlock);

  dict_item_t *ptr = dict_find(&vfs_data.mounts, path, strlen(path));
  if (ptr) {
    struct vfs_item *vi = dict_container(ptr, struct vfs_item, item);
    dict_erase(&vfs_data.mounts, ptr);
    *fs = vi->fs;
    free(vi);
    return 0;
  } else return -1;
}

/* TODO: implement async traverse
int vfs_traverse(const char *path, vfs_node_t *node) {
  GUARD_PTHREAD_RWLOCK_READ(&vfs_data.rwlock);

  vfs_node_t node1, node2; // we need temp space while traversing, to avoid overwrites.
  vfs_node_t *node_ptr = 0;
  vfs_io_t io;
  vfs_file_t file;
  io->opcode = VFS_OPCODE_FIND;
  io->priority = 0;
  const char *prev = path;
  const char *curr = strchr(prev, '/');
  size_t len = curr ? (curr == prev ? 1 : (size_t)(curr - path)) : strlen(path);
  while (*prev) {
    // TODO: if *node_ptr is a symbolic link, traverse the symbol path first.
    dict_item_t *ptr = dict_find(&vfs_data.mounts, path, len);
    vfs_node_t *next_node = node_ptr == &node1 ? &node2 : &node1;
    if (ptr) { // mount point found.
      struct vfs_item *item = dict_container(ptr, struct vfs_item, item);
      item->fs.op.root(&item->fs, node_ptr);
      node_ptr = next_node;
    } else if (node_ptr && prev != curr) {
      io->node = next_ptr;
      io->buf = prev;
      io->nbytes = curr - prev;
      
      node_ptr->op.open(node_ptr, O_RDONLY, &file);
      file->op.enqueue(node_ptr, prev, curr - prev, next_tag);
      node_ptr = next_node;
    }
    prev = curr + 1;
    curr = strchr(prev, '/');
    if (!curr) curr = prev + strlen(prev);
    len = curr - path;
  }

  if (tag_ptr) {
    *tag = *tag_ptr;
    return 0;
  } else return -1;
}
*/

void vfs_bind(vfs_t *vfs) {
  *vfs = vfs_data.vfs;
}

void vfs_init() {
  vfs_data.rwlock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
  dict_init(&vfs_data.mounts);

  BROKER_REGISTER(VFS_ID, REGISTER_SERVICE(vfs_bind));
  vfs_data.vfs.mount   = REGISTER_SERVICE(vfs_mount);
  vfs_data.vfs.unmount = REGISTER_SERVICE(vfs_unmount);
}
