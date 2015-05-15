#ifndef _CORESRV_VFS_H_
#define _CORESRV_VFS_H_

#include <coresrv/coresrv.h>

#include <sys/types.h>

typedef struct vfo vfo_t;

__IDC__ int coresrv_vfs_close(IDC_PTR, vfo_t*);
__IDC__ ssize_t coresrv_vfs_read(IDC_PTR, vfo_t*, void *, size_t);
__IDC__ ssize_t coresrv_vfs_write(IDC_PTR, vfo_t*, const void *, size_t);
__IDC__ off_t coresrv_vfs_lseek(IDC_PTR, vfo_t*, off_t, int);
__IDC__ int coresrv_vfs_fsync(IDC_PTR, vfo_t*);
__IDC__ int coresrv_vfs_ftruncate(IDC_PTR, vfo_t*, off_t);

typedef struct vfo {
  coresrv_vfs_close_ptr close;
  coresrv_vfs_read_ptr read;
  coresrv_vfs_write_ptr write;
  coresrv_vfs_lseek_ptr lseek;
  coresrv_vfs_fsync_ptr fsync;
  coresrv_vfs_ftruncate_ptr ftruncate;

  uintptr_t id;
} vfo_t;

__IDC__ int coresrv_vfs_open(IDC_PTR, vfo_t*, const char *name, int flags);

#endif
