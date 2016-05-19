#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <hellbender/fs/vfs.h>

int rmdir(const char *path) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return unlinkat(fd, path, AT_REMOVEDIR);
}
