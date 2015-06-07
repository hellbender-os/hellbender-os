#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

int mkfifo(const char *path, mode_t mode) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return mkfifoat(fd, path, mode);
}

int mkfifoat(int fd, const char *path, mode_t mode) {
  struct vfs_file *dir = NULL;
  if (fd >= 0 && fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[fd];
  }
  if (strlen(path) == 0 && dir) {
    errno = ENOENT;
    return -1;
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path, O_CREAT|S_IFIFO) != 0) {
    return -1;
  }
  if (file.stat.st_mode) {
    errno = EEXIST;
    goto error;
  }
  if (!file.filesys.create) {
    errno = EROFS;
    goto error;
  }
  mode &= _fcntl_data.umask;
  if (IDC(vfs_create, file.filesys.create, &file, mode|S_IFIFO) != 0) {
    goto error;
  }
  return 0;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}
