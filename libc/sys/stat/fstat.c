#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "../fcntl/fcntl_impl.h"

int fstat(int handle, struct stat *buf) {
  if (handle < 0 || handle >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.fstat) {
    *buf = file->stat;
  } else if (IDC(vfs_fstat, file->filesys.fstat, file, buf) != 0) {
    return -1;
  }
  return 0;
}

int fstatat(int fd, const char *path, struct stat *buf, int flag) {
  if (flag && flag != AT_SYMLINK_NOFOLLOW) {
    errno = EINVAL;
    return -1;
  }
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  struct vfs_file *dir = NULL;
  if (fd >= 0 && fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[fd];
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path, F_OK | flag) != 0) {
    return -1;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  if (!file.filesys.fstat) {
    *buf = file.stat;
  } else if (IDC(vfs_fstat, file.filesys.fstat, &file, buf) != 0) {
    goto error;
  }

  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return 0;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}

int lstat(const char *path, struct stat *buf) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fstatat(fd, path, buf, AT_SYMLINK_NOFOLLOW);
}

int stat(const char *path, struct stat *buf) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fstatat(fd, path, buf, 0);
}
