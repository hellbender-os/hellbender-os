#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

char *ttyname(int fildes) {
  static char* buffer = 0;
  if (!buffer) buffer = malloc(PATH_MAX+1);
  errno = ttyname_r(fildes, buffer, PATH_MAX);
  return errno ? 0 : buffer;
}

int ttyname_r(int fildes, char *name, size_t namesize) {
  if (!isatty(fildes)) return errno;
  vfs_file_t *file = _fcntl_get_file(fildes);
  errno = vfs.get_path(file, name, namesize);
  return errno;
}
