#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <hellbender/fcntl_impl.h>

int chmod(const char *path, mode_t mode) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fchmodat(fd, path, mode, 0);
}

int fchmodat(int fd, const char *path, mode_t mode, int flag) {
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
  errno = vfs.open(dir, path, 0, flag, 0, &file);
  if (errno) return -1;
  errno = vfs.chmod(&file, mode);
  file.op.close(&file);
  return errno ? -1 : 0;
}

int fchmod(int fildes, mode_t mode) {
  vfs_file_t *file = _fcntl_get_file(fildes);
  if (!file || !file->oflags) {
    errno = EBADF;
    return -1;
  }
  errno = vfs.chmod(file, mode);
  return errno ? -1 : 0;
}
