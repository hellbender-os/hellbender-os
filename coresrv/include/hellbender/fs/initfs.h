#ifndef __CORESRV_FS_INITFS_H__
#define __CORESRV_FS_INITFS_H__

#include "vfs.h"

#include <stdint.h>
#include <stddef.h>

typedef struct initfs {
  int (*create)(void *initrd_base, size_t initrd_size, vfs_tag_t *root);
} initfs_t;

typedef void (*initfs_bind_t)(initfs_t *vfs);
#define INITFS_ID "coresrv::fs::initfs"
#define INITFS_BIND ((initfs_bind_t)BROKER_LOOKUP(INITFS_ID))

#endif
