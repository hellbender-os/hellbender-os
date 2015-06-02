#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include <coresrv/vfs.h>
#include <coresrv/devfs.h>
#include <kernel/kernel.h>

void  vfs_devfs_init(struct vfs_devfs *devfs) {
  memset(devfs, 0, sizeof(struct vfs_devfs));
  //devfs->filesys.create = MAKE_IDC_PTR(vfs_create, vfs_devfs_create);
  devfs->filesys.open = MAKE_IDC_PTR(vfs_open, vfs_devfs_open);
  devfs->filesys.close = MAKE_IDC_PTR(vfs_close, vfs_devfs_close);
  devfs->filesys.read = MAKE_IDC_PTR(vfs_read, vfs_devfs_read);
  //devfs->filesys.write = MAKE_IDC_PTR(vfs_write, vfs_devfs_write);
  devfs->filesys.lseek = MAKE_IDC_PTR(vfs_lseek, vfs_devfs_lseek);
  //devfs->filesys.fsync = MAKE_IDC_PTR(vfs_fsync, vfs_devfs_fsync);
  //devfs->filesys.ftruncate = MAKE_IDC_PTR(vfs_ftruncate, vfs_devfs_ftruncate);
  //devfs->filesys.fstat = MAKE_IDC_PTR(vfs_fstat, vfs_devfs_fstat);
  devfs->filesys.internal = devfs;
}

int vfs_devfs_add(struct vfs_devfs *devfs,
                  const char* name, struct vfs_filesys *filesys) {
  struct vfs_devfs_entry *entry =
    (struct vfs_devfs_entry *)malloc(sizeof(struct vfs_devfs_entry));
  entry->next = devfs->first;
  entry->filesys = *filesys;
  if (strcpy_s((char*)entry->name, NAME_MAX+1, name)) {
    free(entry);
    return -1;
  }
  devfs->first = entry;
  devfs->n_devices++;
  return 0;
}

__IDCIMPL__ int vfs_devfs_open(IDC_PTR, struct vfs_file* file, const char *name, int flags) {
  (void)(flags); // TODO: check flags.
  struct vfs_devfs *devfs = (struct vfs_devfs *)file->filesys.internal;
  
  if (strlen(name) == 0) {
    // root of the devfs lists the registered devices.
    file->internal = malloc(sizeof(struct vfs_devfs_file));
    struct vfs_devfs_file *this = (struct vfs_devfs_file*)file->internal;
    this->current = devfs->first;
    this->offset = 0;
    file->stat.st_mode = S_IFDIR;
    return 0;

  } else {
    // find device by name.
    for (struct vfs_devfs_entry *ptr = devfs->first; 
         ptr; ptr = ptr->next) {
      if (strcmp(ptr->name, name) == 0) {
        // open the device.
        vfs_devfs_close(NO_IDC, file);
        file->filesys = ptr->filesys;
        file->stat.st_mode = S_IFCHR;
        return IDC(vfs_open, file->filesys.open, file, name, flags);
      }
    }
  }
  return -1;
}

__IDCIMPL__ int vfs_devfs_close(IDC_PTR, struct vfs_file* file) {
  memset(&file->filesys, 0, sizeof(file->filesys));
  free(file->internal);
  file->internal = NULL;
  file->stat.st_mode = 0;
  return 0;
}

__IDCIMPL__ ssize_t vfs_devfs_read(IDC_PTR, struct vfs_file* file, void *buf, size_t size) {
  struct vfs_devfs_file *internal = (struct vfs_devfs_file*)file->internal;
  if (size != sizeof(struct dirent)) {
    printf("Directories only support struct dirent.\n");
    return 0;
  }
  if (internal->current) {
    struct dirent* de = (struct dirent*) buf;
    de->d_ino = (ino_t)internal->current;
    strcpy_s(de->d_name, NAME_MAX+1, internal->current->name);
    internal->current = internal->current->next;
    internal->offset += size;
    return size;

  } else {
    return 0;
  }
}

__IDCIMPL__ off_t vfs_devfs_lseek(IDC_PTR, struct vfs_file* file, off_t off, int where) {
  struct vfs_devfs_file *internal = (struct vfs_devfs_file*)file->internal;
  struct vfs_devfs *devfs = (struct vfs_devfs *)file->filesys.internal;

  switch (where) {
  case SEEK_SET:
    internal->offset = off;
    break;
  case SEEK_CUR:
    internal->offset += off;
    break;
  case SEEK_END:
    internal->offset = devfs->n_devices * sizeof(struct dirent);
    break;
  }

  // find the matching device.
  off_t i = internal->offset;
  off_t j = 0;
  internal->current = devfs->first;
  while (i >= (int)sizeof(struct dirent) && internal->current) {
    i -= sizeof(struct dirent);
    j += sizeof(struct dirent);
    internal->current = internal->current->next;
  }
  internal->offset = j;

  return internal->offset;
}
