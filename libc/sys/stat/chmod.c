#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

int chmod(const char *path, mode_t mode) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fchmodat(fd, path, mode, 0);
}

int fchmodat(int fd, const char *path, mode_t mode, int flag) {
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
  if (!file.filesys.chmod) {
    errno = EROFS;
    goto error;
  }
  if (IDC(vfs_chmod, file.filesys.chmod, &file, mode) != 0) {
    goto error;
  }
  return 0;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}

int fchmod(int fildes, mode_t mode) {
  if (fildes < 0 || fildes >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[fildes];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.chmod) {
    errno = EROFS;
    return -1;
  }
  if (IDC(vfs_chmod, file->filesys.chmod, file, mode) != 0) {
    return -1;
  }
  return 0;
}
