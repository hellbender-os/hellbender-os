#ifndef __CORESRV_FS_VFS_H__
#define __CORESRV_FS_VFS_H__

#include <sys/types.h>
#include <sys/stat.h>

typedef struct vfs_iter vfs_iter_t;
typedef struct vfs_tag vfs_tag_t;
typedef struct vfs_node vfs_node_t;

struct vfs_tag_op {
  int (*find)(vfs_tag_t *this, const char *name, int name_len, vfs_tag_t *tag);
  int (*list)(vfs_tag_t *this, vfs_iter_t *iter);
  int (*node)(vfs_tag_t *this, vfs_node_t *node);
};
typedef struct vfs_tag {
  ino_t ino;
  uid_t uid;
  gid_t gid;
  mode_t mode;
  struct vfs_tag_op op;
  struct { void *data; } impl;
} vfs_tag_t;

typedef struct vfs_iter {
  vfs_tag_t tag;
  const char name[NAME_MAX+1];
  int (*next)();
  struct { void *data; } impl;
} vfs_iter_t;

struct vfs_node_op {
  int (*open)(vfs_node_t *this, const char *name, int flags, mode_t mode);
  int (*close)(vfs_node_t *this);
  ssize_t (*read)(vfs_node_t *this, void *buf, size_t nbyte, off_t offset);
  ssize_t (*write)(vfs_node_t *this, void *buf, size_t nbyte, off_t offset);
  int (*stat)(vfs_node_t *this, struct stat *buf);
  int (*mknod)(vfs_node_t *this, const char *name, mode_t mode);
  int (*chown)(vfs_node_t *this, const char *name, uid_t owner, gid_t group);
  int (*chmod)(vfs_node_t *this, const char *name, mode_t mode, int flag);
  void* (*query)(vfs_node_t *this, int iface_id);
};
typedef struct vfs_node {
  vfs_tag_t tag;
  struct vfs_node_op op;
  struct { void *data; } impl;
} vfs_node_t;

typedef struct vfs {
  int (*mount)(const char *path, vfs_tag_t *root);
  int (*unmount)(const char *path, vfs_node_t *root);
  int (*traverse)(const char *path, vfs_node_t *node);
} vfs_t;

typedef void (*vfs_bind_t)(vfs_t *vfs);
#define VFS_ID "coresrv::fs::vfs"
#define VFS_BIND ((vfs_bind_t)BROKER_LOOKUP(VFS_ID))

#endif
