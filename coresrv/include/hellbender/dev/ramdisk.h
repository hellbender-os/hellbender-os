#ifndef __CORESRV_DEV_RAMDISK_H__
#define __CORESRV_DEV_RAMDISK_H__

#include "bdev.h"

#include <stddef.h>

typedef struct ramdisk_op {
  int (*create)(size_t size, vfs_node_t *node);
} ramdisk_op_t;

typedef void (*ramdisk_bind_t)(ramdisk_op_t *ramdisk_op);
#define RAMDISK_ID "coresrv::fs::ramdisk"
#define RAMDISK_BIND ((ramdisk_bind_t)BROKER_LOOKUP(RAMDISK_ID))

#endif
