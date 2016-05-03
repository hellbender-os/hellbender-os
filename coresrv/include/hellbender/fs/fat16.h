#ifndef __CORESRV_FS_FAT16_H__
#define __CORESRV_FS_FAT16_H__

#include <hellbender/fs/vfs.h>
#include <hellbender/dev/bdev.h>

typedef struct fat16 {
  int (*create)(vfs_tag_t *dev, vfs_tag_t *root);
} fat16_t;

typedef void (*fat16_bind_t)(fat16_t *vfs);
#define FAT16_ID "coresrv::fs::fat16"
#define FAT16_BIND ((fat16_bind_t)BROKER_LOOKUP(FAT16_ID))

#endif
