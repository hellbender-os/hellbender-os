#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "../fcntl/fcntl_impl.h"

DIR *fdopendir(int fd) {
  if (fd < 0 || fd > OPEN_MAX) {
    errno = EBADF;
    return NULL;
  }
  struct vfs_file *file = &_fcntl_data.handles[fd];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return NULL;
  }
  if (!S_ISDIR(file->stat.st_mode)) {
    errno = ENOTDIR;
    return NULL;
  }
  DIR *dir = malloc(sizeof(DIR));
  dir->fd = fd;
  return dir;
}

DIR *opendir(const char *dirname) {
  if (strlen(dirname) == 0) {
    errno = ENOENT;
    return NULL;
  }
  int fd = open(dirname, O_RDONLY | O_DIRECTORY);
  if (fd == -1) {
    return NULL;
  }
  struct vfs_file *file = &_fcntl_data.handles[fd];
  if (!file->stat.st_mode) {
    errno = ENOENT;
    goto error;
  }
  if (!S_ISDIR(file->stat.st_mode)) {
    errno = ENOTDIR;
    goto error;
  }
  DIR *dir = malloc(sizeof(DIR));
  dir->fd = fd;
  return dir;

 error:
  close(fd);
  return NULL;
}
