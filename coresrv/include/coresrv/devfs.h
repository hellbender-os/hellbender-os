#ifndef _CORESRV_DEVFS_H
#define _CORESRV_DEVFS_H

#include <stdint.h>
#include <limits.h>

#include <coresrv/vfs.h>

struct vfs_devfs {
  struct vfs_filesys filesys;

  struct vfs_devfs_entry {
    struct vfs_devfs_entry *next;
    char name[NAME_MAX+1];
    struct vfs_filesys filesys;
  } *first;
  size_t n_devices;
};

struct vfs_devfs_file {
  struct vfs_devfs_entry *current;
  off_t offset;
};

void vfs_devfs_init(struct vfs_devfs *);
int vfs_devfs_add(struct vfs_devfs *, const char*, struct vfs_filesys*);

int vfs_devfs_open(IDC_PTR, struct vfs_file*, const char*, int);
int vfs_devfs_close(IDC_PTR, struct vfs_file*);
ssize_t vfs_devfs_read(IDC_PTR, struct vfs_file*, void*, size_t);


#endif
