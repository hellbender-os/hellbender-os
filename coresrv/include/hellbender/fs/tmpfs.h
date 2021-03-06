#ifndef __CORESRV_FS_TMPFS_H__
#define __CORESRV_FS_TMPFS_H__

#include "vfs.h"

#include <stdint.h>
#include <stddef.h>

typedef struct tmpfs_op {
  int (*create)(size_t max_size, dev_t dev, vfs_fs_t *fs);
} tmpfs_op_t;

typedef void (*tmpfs_bind_t)(tmpfs_op_t *tmpfs_op);
#define TMPFS_ID "coresrv::fs::tmpfs"
#define TMPFS_BIND ((tmpfs_bind_t)BROKER_LOOKUP(TMPFS_ID))

#endif
