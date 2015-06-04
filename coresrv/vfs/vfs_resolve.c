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

__IDCIMPL__ int vfs_resolve(IDC_PTR, struct vfs_file *dir, struct vfs_file *file, const char *name, int flags) {
  // start from the root.
  int in_root = 1;
  file->oflags = flags;
  file->owner = -1;
  file->filesys = vfs_rootfs.filesys;
  memset(&file->stat, 0, sizeof(struct stat));
  file->stat.st_mode = S_IFDIR;
  file->real_path[0] = 0;
  
  vfs_rootfs_open_mount(&vfs_rootfs, file, "");
  const char* remname = name;
  char thisname[NAME_MAX+2]; // room for directory separator.
  char fullpath[PATH_MAX] = "";

  if (dir) {
    if (vfs_resolve(NO_IDC, NULL, file, dir->real_path, flags) != 0) return -1;
    strcpy(fullpath, dir->real_path);
  }

  while (*remname) {
    remname = get_next_name(remname, thisname); // copies name until separator.
    if (strcat_s(fullpath, PATH_MAX, thisname)) return -1;

    if (IDC(vfs_open, file->filesys.open, file, thisname, flags)) return -1;
    // path was found in the current file system, check mount points.
    if (!in_root) {
      if (vfs_rootfs_open_mount(&vfs_rootfs, file, fullpath) == 0) {
        // ok, we just switched over to a new filesystem.
      }
    }
    in_root = 0;
  }
  
  return 0;
}

