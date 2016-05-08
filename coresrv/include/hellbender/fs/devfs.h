#ifndef __CORESRV_FS_DEVFS_H__
#define __CORESRV_FS_DEVFS_H__

#include "vfs.h"

#include <stdint.h>
#include <stddef.h>

typedef struct cdev cdev_t;
typedef struct bdev bdev_t;

typedef struct devfs_op {
  int (*create)(vfs_fs_t *fs);
  int (*add_dev)(vfs_fs_t *fs, const char *name, vfs_node_t *node, dev_t *dev);
  dev_t (*alloc_dev)();
} devfs_op_t;

typedef void (*devfs_bind_t)(devfs_op_t *devfs_op);
#define DEVFS_ID "coresrv::fs::devfs"
#define DEVFS_BIND ((devfs_bind_t)BROKER_LOOKUP(DEVFS_ID))

#endif
