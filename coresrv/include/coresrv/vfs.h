#ifndef _CORESRV_VFS_H_
#define _CORESRV_VFS_H_

#include <coresrv/coresrv.h>

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

#define VFS_SEPARATOR_CHAR '/'
#define VFS_SEPARATOR_STRING "/"

struct vfs_filesys {
  vfs_create_ptr create;
  vfs_open_ptr open;
  vfs_close_ptr close;
  vfs_read_ptr read;
  vfs_write_ptr write;
  vfs_lseek_ptr lseek;
  vfs_fsync_ptr fsync;
  vfs_ftruncate_ptr ftruncate;
  vfs_fstat_ptr fstat;

  void* internal;
};

struct vfs_file {
  unsigned in_use;
  int oflags;
  bool isatty;
  id_t owner; // group id or process id; for sockets.
  struct vfs_filesys filesys;
  struct stat stat;
  char real_path[PATH_MAX];
  
  void *internal;
};

struct stat;

extern struct vfs_rootfs vfs_rootfs;

void vfs_init();

// operation on the whole filesystem:
__IDC__ int vfs_mount(IDC_PTR, const char*, struct vfs_filesys *);
__IDC__ int vfs_unmount(IDC_PTR, const char*);
__IDC__ int vfs_resolve(IDC_PTR, struct vfs_file*, struct vfs_file*, const char *, int);

// operation on a vfs_file:
__IDC__ int vfs_create(IDC_PTR, struct vfs_file*, int);
__IDC__ int vfs_open(IDC_PTR, struct vfs_file*, const char*, int);
__IDC__ int vfs_close(IDC_PTR, struct vfs_file*);
__IDC__ ssize_t vfs_read(IDC_PTR, struct vfs_file*, void *, size_t);
__IDC__ ssize_t vfs_write(IDC_PTR, struct vfs_file*, const void *, size_t);
__IDC__ off_t vfs_lseek(IDC_PTR, struct vfs_file*, off_t, int);
__IDC__ int vfs_fsync(IDC_PTR, struct vfs_file*);
__IDC__ int vfs_ftruncate(IDC_PTR, struct vfs_file*, off_t);
__IDC__ int vfs_fstat(IDC_PTR, struct vfs_file*, struct stat*);

#endif
