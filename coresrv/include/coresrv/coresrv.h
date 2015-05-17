#ifndef _CORESRV_CORESRV_H
#define _CORESRV_CORESRV_H

#include <stdint.h>
#include <sys/types.h>

#include <kernel/idc.h>

struct vfs_file;
struct vfs_filesys;
struct vfs_rootfs;
struct vfs_initfs;

#include <coresrv/coresrv_idc.h>

// calls the default coresrv implementation of an IDC interface.
#define CORE_IDC(IFACE, ...) __IDC__ ## IFACE (&__IDE__ ## IFACE, ##__VA_ARGS__)

#endif
