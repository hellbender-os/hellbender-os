#ifndef __CORESRV_DEV_BDEV_H__
#define __CORESRV_DEV_BDEV_H__

#include <hellbender/fs/vfs.h>

struct bdev;

struct bdev_op {
  // get block size, etc.
};

typedef struct bdev {
  struct bdev_op op;
  struct vfs_impl impl;
} bdev_t;

#endif
