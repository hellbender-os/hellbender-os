#ifndef __CORESRV_FS_VFS_H__
#define __CORESRV_FS_VFS_H__

#include <sys/types.h>
#include <sys/stat.h>

#include <stdint.h>

#define VFS_IFACE_CDEV 1 // query for interface cdev_t
#define VFS_IFACE_BDEV 2 // query for interface bdev_t

#define VFS_OPCODE_CLOSE   1
#define VFS_OPCODE_FLUSH   2
#define VFS_OPCODE_READ    3 // buf == buffer to write nbytes data into.
#define VFS_OPCODE_WRITE   4 // buf == buffer to read nbytes data from.
#define VFS_OPCODE_READDIR 5 // buf == struct dirent[]; nbytes = n * sizeof(struct dirent)
#define VFS_OPCODE_MKNOD   6 // buf == struct vfs_mknod.
#define VFS_OPCODE_UNLINK  7 // buf == filename; nbytes == stdlen(filename)
#define VFS_OPCODE_FIND    8 // buf == filename; nbytes == stdlen(filename); updates node.

#define VFS_CANCEL_ALLDONE 0

typedef struct vfs_fs vfs_fs_t;
typedef struct vfs_node vfs_node_t;
typedef struct vfs_file vfs_file_t;

struct vfs_mknod {
  ino_t  ino;              // Node to link to.
  char   name[NAME_MAX+1];
  int    name_len;
  mode_t mode;
  dev_t  rdev; // if mode == S_IFBLK or S_IFCHR.
};

struct vfs_impl {
  uint64_t data[4];
};

typedef struct vfs_io {
  vfs_node_t *node;   // updated to the new node (when VFS_OPCODE_FIND).
  int         opcode;
  off_t       offset; // updated to the next available offset (on success).
  void       *buf;    // data buffer, dirent buffer, filename (depends on opcode).
  size_t      nbytes; // updated to the actual number of bytes read/written (on success).
  int         priority;
  void (*onsuccess)(struct vfs_io *);
  void (*onfailure)(struct vfs_io *, int);
  struct vfs_impl impl;
} vfs_io_t;

typedef struct vfs_fs {
  struct vfs_fs_op {
    int (*root)(vfs_fs_t *this, vfs_node_t *node);
  } op;
  struct vfs_impl impl;
} vfs_fs_t;

typedef struct vfs_node {
  struct stat stat;
  struct vfs_node_op {
    int (*open)(vfs_node_t *this, int flags, vfs_file_t *file);
    int (*query)(vfs_node_t *this, int iface_id, void **iface);
  } op;
  struct vfs_impl impl;
} vfs_node_t;

typedef struct vfs_file {
  int oflags;
  struct vfs_file_op {
    int (*enqueue)(vfs_file_t *this, vfs_io_t *io);
    int (*cancel)(vfs_file_t *this, vfs_io_t *io);
  } op;
  struct vfs_impl impl;
} vfs_file_t;

typedef struct vfs {
  int (*mount)(const char *path, vfs_fs_t *fs);
  int (*unmount)(const char *path, vfs_fs_t *fs);
  // TODO: add all operations to support fcnt, unistd
} vfs_t;

typedef void (*vfs_bind_t)(vfs_t *vfs);
#define VFS_ID "coresrv::fs::vfs"
#define VFS_BIND ((vfs_bind_t)BROKER_LOOKUP(VFS_ID))

#endif
