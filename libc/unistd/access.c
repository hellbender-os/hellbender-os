#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "../fcntl/fcntl_impl.h"

int access(const char *path, int amode) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return faccessat(fd, path, amode, 0);
}

int faccessat(int fd, const char *path, int amode, int flag) {
  if (strlen(path) == 0 && fd != -1) {
    errno = ENOENT;
    return -1;
  }
  if (flag && flag != AT_EACCESS) {
    errno = EINVAL;
    return -1;
  }
  struct vfs_file *dir = NULL;
  if (fd >= 0 && fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[fd];
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path, amode | flag) != 0) {
    return -1;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return 0;
}
