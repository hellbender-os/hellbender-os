#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <hellbender/fcntl_impl.h>

int mkfifo(const char *path, mode_t mode) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return mkfifoat(fd, path, mode);
}

int mkfifoat(int fd, const char *path, mode_t mode) {
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
  errno = vfs.open(dir, path, O_CREAT|O_EXCL, 0, (mode & _fcntl.umask) | S_IFIFO, &file);
  if (!errno) file.op.close(&file);
  return errno ? -1 : 0;
}
