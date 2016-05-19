#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

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
  if (flag & ~AT_REMOVEDIR) {
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
  if (flag & AT_REMOVEDIR || path[strlen(path)-1] == VFS_SEPARATOR_CHAR) {
    errno = vfs.open(dir, path, O_DIRECTORY, 0, 0, &file);
  } else {
    errno = vfs.open(dir, path, 0, 0, 0, &file);
  }
  if (errno) return -1;
  errno = vfs.unlink(&file);
  file.op.close(&file);
  return errno ? -1 : 0;
}
