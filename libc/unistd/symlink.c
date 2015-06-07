#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

int symlink(const char *path1, const char *path2) {
  int fd = -1;
  if (path2[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return symlinkat(path1, fd, path2);
}

int symlinkat(const char *path1, int fd, const char *path2) {
  struct vfs_file *dir = NULL;
  if (fd >= 0 && fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[fd];
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path2, O_CREAT | O_EXCL | O_WRONLY) != 0) {
    return -1;
  }
  if (file.stat.st_mode) {
    errno = EEXIST;
    return -1;
  }
  if (!file.filesys.create || !file.filesys.write) {
    errno = EROFS;
    goto error;
  }
  if (IDC(vfs_create, file.filesys.create, &file, S_IRUSR|S_IRGRP|S_IROTH) != 0) {
    goto error;
  }
  if (IDC(vfs_write, file.filesys.write, &file, path1, strlen(path1))) {
    goto error;
  }
  return 0;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}
