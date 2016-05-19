#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

ssize_t readlink(const char *path, char *buf, size_t bufsize) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return readlinkat(fd, path, buf, bufsize);
}

ssize_t readlinkat(int fd, const char *path, char *buf, size_t bufsize) {
  if (strlen(path) == 0) {
    errno = ENOENT;
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
  errno = vfs.open(dir, path, O_RDONLY, AT_SYMLINK_NOFOLLOW, 0, &file);
  if (errno) return -1;
  if (S_ISLNK(file.st_mode)) {
    errno = file.op.read(&file, 0, buf, &bufsize);
  } else {
    errno = EINVAL;
  }
  file.op.close(&file);
  return errno ? -1 : (ssize_t)bufsize;
}

