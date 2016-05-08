#ifndef __CORESRV_DEV_CDEV_H__
#define __CORESRV_DEV_CDEV_H__

#include <hellbender/fs/vfs.h>

struct cdev;

struct cdev_op {
  // TODO isatty etc.
};

typedef struct cdev {
  struct cdev_op op;
  struct vfs_impl impl;
} cdev_t;

#endif
