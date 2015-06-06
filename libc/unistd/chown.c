#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

static int dochown(struct vfs_file *file, uid_t user, gid_t group) {
  if (!file->filesys.chown) {
    errno = EROFS;
    return -1;
  }
  // TODO: check effective user id.
  if (IDC(vfs_chown, file->filesys.chown, file, user, group) != 0) {
    return -1;
  }
  return 0;
}

int chown(const char *path, uid_t user, gid_t group) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fchownat(fd, path, user, group, 0);
}

int lchown(const char *path, uid_t user, gid_t group) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fchownat(fd, path, user, group, AT_SYMLINK_NOFOLLOW);
}

int fchown(int fd, uid_t user, gid_t group) {
  if (fd < 0 || fd >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[fd];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  return dochown(file, user, group);
}

int fchownat(int fd, const char *path, uid_t user, gid_t group, int flags) {
  if (flags && flags != AT_SYMLINK_NOFOLLOW) {
    errno = EINVAL;
    return -1;
  }
  struct vfs_file *dir = NULL;
  if (fd >= 0 && fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[fd];
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path, F_OK | flags) != 0) {
    return -1;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  return dochown(&file, user, group);
}
