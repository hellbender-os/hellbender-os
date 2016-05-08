#ifndef __CORESRV_FS_INITFS_H__
#define __CORESRV_FS_INITFS_H__

#include "vfs.h"

#include <stdint.h>
#include <stddef.h>

typedef struct initfs_op {
  int (*create)(void *initrd_base, size_t initrd_size, dev_t dev, vfs_fs_t *fs);
} initfs_op_t;

typedef void (*initfs_bind_t)(initfs_op_t *initfs_op);
#define INITFS_ID "coresrv::fs::initfs"
#define INITFS_BIND ((initfs_bind_t)BROKER_LOOKUP(INITFS_ID))

#endif
