#include <stdio.h>
#include <string.h>
#include <coresrv/vfs.h>
#include <coresrv/rootfs.h>
#include <kernel/kernel.h>

static const char* get_next_name(const char* path, char *name) {
  unsigned i = 0;
  for (; i < NAME_MAX && path[i] && path[i] != VFS_SEPARATOR_CHAR; ++i) {
    name[i] = path[i];
  }
  if (path[i] == VFS_SEPARATOR_CHAR) {
    name[i] = VFS_SEPARATOR_CHAR;
    name[i+1] = 0;
    return path + i + 1;
  } else {
    name[i] = 0;
    return path + i;
  }
}

__IDCIMPL__ int vfs_open(IDC_PTR, struct vfs_file *file, const char *name, int flags) {
  memset(file, 0, sizeof(struct vfs_file));

  // start from the root.
  file->filesys = &vfs_rootfs.filesys;
  vfs_rootfs_open_mount(&vfs_rootfs, file, "");
  const char* remname = name;
  char thisname[NAME_MAX+2]; // room for directory separator.
  char fullpath[PATH_MAX] = "";

  while (*remname) {
    remname = get_next_name(remname, thisname); // copies name until separator.
    if (strcat_s(fullpath, PATH_MAX, thisname)) return -1;
    struct vfs_filesys *last_fs = file->filesys;
    if (IDC(vfs_open, file->filesys->open, file, thisname, flags)) return -1;
    // path was found in the current file system, check mount points.
    if (last_fs != &vfs_rootfs.filesys) {
      if (vfs_rootfs_open_mount(&vfs_rootfs, file, fullpath) == 0) {
        // ok, we just switched over to a new filesystem.
      }
    }
  }
  
  return 0;
}

