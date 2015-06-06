#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "../fcntl/fcntl_impl.h"

int truncate(const char *path, off_t length) {
  if (length < 0) {
    errno = EINVAL;
    return -1;
  }
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  struct vfs_file *dir = NULL;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    dir = &_fcntl_data.handles[AT_FDCWD];
  }
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, dir, &file, path, W_OK) != 0) {
    return -1;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  if (!file.filesys.ftruncate) {
    errno = EROFS;
    goto error;
  }
  if (IDC(vfs_ftruncate, file.filesys.ftruncate, &file, length) != 0) {
    goto error;
  }
  return 0;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return -1;
}
