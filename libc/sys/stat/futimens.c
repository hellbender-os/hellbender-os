#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

int futimens(int fd, const struct timespec times[2]) {
  if (fd < 0 || fd >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[fd];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.utimes) {
    errno = EROFS;
    return -1;
  }
  if (IDC(vfs_utimes, file->filesys.utimes, file, times) != 0) {
    return -1;
  }
  return 0;
}

int utimensat(int fd, const char *path, const struct timespec times[2],
              int flag) {
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
  if (CORE_IDC(vfs_resolve, dir, &file, path, F_OK) != 0) {
    return -1;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  if (!file.filesys.utimes) {
    errno = EROFS;
    goto error;
  }
  if (IDC(vfs_utimes, file.filesys.utimes, &file, times) != 0) {
    goto error;
  }
  return 0;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}

