#ifndef _CORESRV_INITFS_H
#define _CORESRV_INITFS_H

#include <stdint.h>
#include <limits.h>

#include <coresrv/vfs.h>

struct vfs_initfs_header {
  uint16_t magic;
  uint16_t dev;
  uint16_t ino;
  uint16_t mode;
  uint16_t uid;
  uint16_t gid;
  uint16_t nlink;
  uint16_t rdev;
  uint16_t mtime_hi;
  uint16_t mtime_lo;
  uint16_t namesize;
  uint16_t filesize_hi;
  uint16_t filesize_lo;
} __attribute__((packed));

struct vfs_initfs {
  struct vfs_filesys filesys;
  uint8_t *buffer;
  size_t size;

  struct vfs_initfs_entry {
    struct vfs_initfs_entry *next;
    struct vfs_initfs_entry *children;
    struct vfs_initfs_entry *flat_next;
    struct vfs_initfs_header header;
    const char *name;
    const char *path;
    const uint8_t *data;
    size_t filesize;
  } root, *flat_first;
};

struct vfs_initfs_file {
  struct vfs_initfs_entry *this;
  struct vfs_initfs_entry *child;
  off_t offset;
};

void vfs_initfs_init(struct vfs_initfs*, uint8_t *, size_t);

int vfs_initfs_open(IDC_PTR, struct vfs_file*, const char *, int);
int vfs_initfs_close(IDC_PTR, struct vfs_file*);
ssize_t vfs_initfs_read(IDC_PTR, struct vfs_file*, void *, size_t);
off_t vfs_initfs_lseek(IDC_PTR, struct vfs_file*, off_t, int);

#endif
