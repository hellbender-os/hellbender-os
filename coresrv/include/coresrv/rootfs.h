#ifndef _CORESRV_ROOTFS_H
#define _CORESRV_ROOTFS_H

#include <coresrv/vfs.h>

__IDC__ int coresrv_rootfs_open(IDC_PTR, vfo_t* vfo, const char *name, int flags);
__IDC__ int coresrv_rootfs_close(IDC_PTR, vfo_t* vto);
__IDC__ ssize_t coresrv_rootfs_read(IDC_PTR, vfo_t* vto, void * buf, size_t size);
__IDC__ off_t coresrv_rootfs_lseek(IDC_PTR, vfo_t* vto, off_t off, int where);

#endif
