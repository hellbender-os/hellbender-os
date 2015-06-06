#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

int unlink(const char *path) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return unlinkat(fd, path, 0);
}

int unlinkat(int fd, const char *path, int flag) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  struct vfs_file *dir = NULL;
  if (fd >= 0 && fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[fd];
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path, F_OK) != 0) {
    return -1;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  if (flag == AT_REMOVEDIR) {
    if (!S_ISDIR(file.stat.st_mode)) {
      errno = ENOTDIR;
      goto error;
    }

  } else if (flag == 0) {
    if (S_ISDIR(file.stat.st_mode)) {
      errno = EPERM;
      goto error;
    }
  } else {
    errno = EINVAL;
    goto error;
  }
  if (!file.filesys.unlink) {
    errno = EROFS;
    goto error;
  }
  if (IDC(vfs_unlink, file.filesys.unlink, &file) != 0) {
    goto error;
  }
  return 0;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}
