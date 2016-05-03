#ifndef __CORESRV_FS_DEVFS_H__
#define __CORESRV_FS_DEVFS_H__

#include "vfs.h"

#include <stdint.h>
#include <stddef.h>

typedef struct cdev cdev_t;
typedef struct bdev bdev_t;

typedef struct devfs {
  int (*create)(vfs_tag_t *root);
  int (*add_dev)(vfs_tag_t *root, const char *name, vfs_tag_t *tag);
} devfs_t;

typedef void (*devfs_bind_t)(devfs_t *vfs);
#define DEVFS_ID "coresrv::fs::devfs"
#define DEVFS_BIND ((devfs_bind_t)BROKER_LOOKUP(DEVFS_ID))

#endif
