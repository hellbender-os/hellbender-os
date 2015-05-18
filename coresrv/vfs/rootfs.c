#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include <coresrv/vfs.h>
#include <coresrv/rootfs.h>
#include <kernel/kernel.h>

void  vfs_rootfs_init(struct vfs_rootfs *rootfs) {
  memset(rootfs, 0, sizeof(struct vfs_rootfs));
  rootfs->filesys.open = MAKE_IDC_PTR(vfs_open, vfs_rootfs_open);
  rootfs->filesys.close = MAKE_IDC_PTR(vfs_close, vfs_rootfs_close);
  rootfs->filesys.read = MAKE_IDC_PTR(vfs_read, vfs_rootfs_read);
  //rootfs->filesys.write = MAKE_IDC_PTR(vfs_write, vfs_rootfs_write);
  rootfs->filesys.lseek = MAKE_IDC_PTR(vfs_lseek, vfs_rootfs_lseek);
  //rootfs->filesys.fsync = MAKE_IDC_PTR(vfs_fsync, vfs_rootfs_fsync);
  //rootfs->filesys.ftruncate = MAKE_IDC_PTR(vfs_ftruncate, vfs_rootfs_ftruncate);
  rootfs->filesys.internal = rootfs;
}

int vfs_rootfs_add(struct vfs_rootfs *rootfs,
                    const char* name, struct vfs_filesys *filesys) {
  struct vfs_rootfs_mount *mount =
    (struct vfs_rootfs_mount *)malloc(sizeof(struct vfs_rootfs_mount));
  mount->next = rootfs->first;
  mount->filesys = *filesys;
  if (strcpy_s((char*)mount->name, NAME_MAX+1, name)) {
    free(mount);
    return -1;
  }
  rootfs->first = mount;
  rootfs->n_mounts++;
  return 0;
}

int vfs_rootfs_open_mount(struct vfs_rootfs *rootfs,
                          struct vfs_file* file, const char *name) {
  if (strlen(name) == 0) {
    // root of the rootfs is always first to be opened; and only opened once.
    file->internal = malloc(sizeof(struct vfs_rootfs_file));
    struct vfs_rootfs_file *this = (struct vfs_rootfs_file*)file->internal;
    this->current = rootfs->first;
    this->offset = 0;
    return 0;

  } else {
    // find mount by name.
    for (struct vfs_rootfs_mount *ptr = rootfs->first; 
         ptr; ptr = ptr->next) {
      if (strcmp(ptr->name, name) == 0) {
        // switch over to the mounted filesystem.
        IDC(vfs_close, file->filesys.close, file);
        file->filesys = ptr->filesys;
        return IDC(vfs_open, file->filesys.open, file, "", 0);
      }
    }
  }
  return -1;
}

__IDCIMPL__ int vfs_rootfs_open(IDC_PTR, struct vfs_file* file, const char *name, int flags) {
  (void)(flags); // TODO: check flags.
  struct vfs_rootfs *rootfs = (struct vfs_rootfs *)file->filesys.internal;
  return vfs_rootfs_open_mount(rootfs, file, name);
}

__IDCIMPL__ int vfs_rootfs_close(IDC_PTR, struct vfs_file* file) {
  memset(&file->filesys, 0, sizeof(file->filesys));
  free(file->internal);
  file->internal = NULL;
  return 0;
}

__IDCIMPL__ ssize_t vfs_rootfs_read(IDC_PTR, struct vfs_file* file, void *buf, size_t size) {
  struct vfs_rootfs_file *internal = (struct vfs_rootfs_file*)file->internal;
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

__IDCIMPL__ off_t vfs_rootfs_lseek(IDC_PTR, struct vfs_file* file, off_t off, int where) {
  struct vfs_rootfs_file *internal = (struct vfs_rootfs_file*)file->internal;
  struct vfs_rootfs *rootfs = (struct vfs_rootfs *)file->filesys.internal;

  switch (where) {
  case SEEK_SET:
    internal->offset = off;
    break;
  case SEEK_CUR:
    internal->offset += off;
    break;
  case SEEK_END:
    internal->offset = rootfs->n_mounts * sizeof(struct dirent);
    break;
  }

  // find the matching mount.
  off_t i = internal->offset;
  off_t j = 0;
  internal->current = rootfs->first;
  while (i >= (int)sizeof(struct dirent) && internal->current) {
    i -= sizeof(struct dirent);
    j += sizeof(struct dirent);
    internal->current = internal->current->next;
  }
  internal->offset = j;

  return internal->offset;
}


