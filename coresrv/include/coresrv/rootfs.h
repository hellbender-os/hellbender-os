#ifndef _CORESRV_ROOTFS_H
#define _CORESRV_ROOTFS_H

#include <limits.h>

#include <coresrv/vfs.h>

struct vfs_rootfs {
  struct vfs_filesys filesys;
  struct vfs_rootfs_mount {
    struct vfs_rootfs_mount *next;
    ino_t ino;
    const char name[NAME_MAX+1];
    struct vfs_filesys *filesys;
  } *first;
  ino_t next_ino;
  size_t n_mounts;
};

struct vfs_rootfs_file {
  struct vfs_rootfs_mount *current;
  off_t offset;
};

void vfs_rootfs_init(struct vfs_rootfs *);
int vfs_rootfs_add(struct vfs_rootfs *,
                   const char* name, struct vfs_filesys *filesys);
int vfs_rootfs_open_mount(struct vfs_rootfs *, struct vfs_file*, const char *);

int vfs_rootfs_open(IDC_PTR, struct vfs_file*, const char *, int);
int vfs_rootfs_close(IDC_PTR, struct vfs_file*);
ssize_t vfs_rootfs_read(IDC_PTR, struct vfs_file*, void *, size_t);
off_t vfs_rootfs_lseek(IDC_PTR, struct vfs_file*, off_t, int);

#endif
