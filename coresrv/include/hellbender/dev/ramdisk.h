#ifndef __CORESRV_DEV_RAMDISK_H__
#define __CORESRV_DEV_RAMDISK_H__

#include "bdev.h"

#include <stddef.h>

typedef struct ramdisk {
  int (*create)(size_t size, vfs_tag_t *tag);
} ramdisk_t;

typedef void (*ramdisk_bind_t)(ramdisk_t *vfs);
#define RAMDISK_ID "coresrv::fs::ramdisk"
#define RAMDISK_BIND ((ramdisk_bind_t)BROKER_LOOKUP(RAMDISK_ID))

#endif
