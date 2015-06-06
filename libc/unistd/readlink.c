#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

ssize_t readlink(const char *path, char *buf, size_t bufsize) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return readlinkat(fd, path, buf, bufsize);
}

ssize_t readlinkat(int fd, const char *path, char *buf, size_t bufsize) {
  if (strlen(path) == 0 && fd != -1) {
    errno = ENOENT;
    return -1;
  }
  struct vfs_file *dir = NULL;
  if (fd >= 0 && fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[fd];
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path, O_RDONLY | O_NOFOLLOW) != 0) {
    return -1;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  if (!S_ISLNK(file.stat.st_mode)) {
    errno = EINVAL;
    goto error;
  }
  if (!file.filesys.read) {
    errno = ENOENT;
    goto error;
  }
  ssize_t retval = IDC(vfs_read, file.filesys.read, &file, buf, bufsize);
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return retval;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}

