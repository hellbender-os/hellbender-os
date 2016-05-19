#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

int access(const char *path, int amode) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return faccessat(fd, path, amode, 0);
}

int faccessat(int dir_fd, const char *path, int amode, int atflags) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  if (atflags & ~AT_EACCESS) {
    errno = EINVAL;
    return -1;
  }
  vfs_file_t *dir = _fcntl_get_file(dir_fd);
  if (path[0] == VFS_SEPARATOR_CHAR) {
    dir = 0;
  } else if (!dir || !(dir->oflags & (O_RDONLY | O_SEARCH))) {
    errno = EBADF;
    return -1;
  }
  vfs_file_t file;
  errno = vfs.open(dir, path, 0, atflags, 0, &file);
  if (errno) return -1;
  int oflags = 0;
  if (amode & R_OK) oflags |= O_RDONLY;
  if (amode & W_OK) oflags |= O_WRONLY;
  if (amode & X_OK) oflags |= O_EXEC;
  errno = vfs.access(&file, oflags);
  file.op.close(&file);
  return errno ? -1 : 0;
}
