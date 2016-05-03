#ifndef __CORESRV_DEV_BDEV_H__
#define __CORESRV_DEV_BDEV_H__

#include <hellbender/fs/vfs.h>

struct bdev;

struct bdev_op {
  // get block size, etc.
};

typedef struct bdev {
  vfs_tag_t tag;
  struct bdev_op op;
  struct { void *data; } impl;
} bdev_t;

#endif
