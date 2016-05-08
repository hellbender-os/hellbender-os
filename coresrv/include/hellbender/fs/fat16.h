#ifndef __CORESRV_FS_FAT16_H__
#define __CORESRV_FS_FAT16_H__

#include <hellbender/fs/vfs.h>
#include <hellbender/dev/bdev.h>

typedef struct fat16_op {
  int (*create)(vfs_node_t *dev, vfs_fs_t *fs);
} fat16_op_t;

typedef void (*fat16_bind_t)(fat16_op_t *fat16_op);
#define FAT16_ID "coresrv::fs::fat16"
#define FAT16_BIND ((fat16_bind_t)BROKER_LOOKUP(FAT16_ID))

#endif
