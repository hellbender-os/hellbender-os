#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

char *getcwd(char *buf, size_t size) {
  if (size == 0) {
    errno = EINVAL;
    return NULL;
  }

  struct vfs_file *dir = &_fcntl_data.handles[AT_FDCWD];
  size_t len = strlen(dir->real_path);
  if (size < len + 1) {
    errno = ERANGE;
    return NULL;
  }

  memcpy(buf, dir->real_path, len+1);
  return buf;
}
