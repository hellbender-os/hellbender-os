#ifndef __CORESRV_FS_TMPFS_H__
#define __CORESRV_FS_TMPFS_H__

#include "vfs.h"

#include <stdint.h>
#include <stddef.h>

typedef struct tmpfs {
  int (*create)(size_t max_size, vfs_tag_t *root);
} tmpfs_t;

typedef void (*tmpfs_bind_t)(tmpfs_t *vfs);
#define TMPFS_ID "coresrv::fs::tmpfs"
#define TMPFS_BIND ((tmpfs_bind_t)BROKER_LOOKUP(TMPFS_ID))

#endif
