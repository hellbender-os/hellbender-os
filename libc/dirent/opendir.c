#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <hellbender/fcntl_impl.h>

DIR *fdopendir(int fd) {
  vfs_file_t *file = _fcntl_get_file(fd);
  if (!file || !(file->oflags & O_RDONLY)) {
    errno = EBADF;
    return NULL;
  }
  if (!S_ISDIR(file->st_mode)) {
    errno = ENOTDIR;
    return NULL;
  }
  DIR *dir = malloc(sizeof(*dir));
  if (dir) dir->fd = fd;
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
  DIR *dir = malloc(sizeof(*dir));
  if (dir) dir->fd = fd;
  else close(fd);
  return dir;
}
