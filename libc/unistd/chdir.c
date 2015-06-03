#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

int chdir(const char *path) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  struct vfs_file *root = NULL;
  struct vfs_file *dir = &_fcntl_data.handles[AT_FDCWD];
  if (path[0] != VFS_SEPARATOR_CHAR) {
    root = dir;
  }
  if (CORE_IDC(vfs_resolve, root, dir, path, O_DIRECTORY | O_SEARCH) != 0) {
    return -1;
  }
  return 0;
}

