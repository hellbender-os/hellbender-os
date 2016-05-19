#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <hellbender/fcntl_impl.h>

int fstat(int fildes, struct stat *buf) {
  struct vfs_file *file = _fcntl_get_file(fildes);
  if (!file || !file->oflags) {
    errno = EBADF;
    return -1;
  }
  errno = file->op.stat(file, buf);
  return errno ? -1 : 0;
}

int fstatat(int fd, const char *path, struct stat *buf, int flag) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  if (flag & ~AT_SYMLINK_NOFOLLOW) {
    errno = EINVAL;
    return -1;
  }
  vfs_file_t *dir = _fcntl_get_file(fd);
  if (path[0] == VFS_SEPARATOR_CHAR) {
    dir = 0;
  } else if (!dir || !(dir->oflags & (O_RDONLY | O_SEARCH))) {
    errno = EBADF;
    return -1;
  }
  vfs_file_t file;
  errno = vfs.open(dir, path, 0, 0, 0, &file);
  if (errno) return -1;
  errno = file.op.stat(&file, buf);
  file.op.close(&file);
  return errno ? -1 : 0;
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
