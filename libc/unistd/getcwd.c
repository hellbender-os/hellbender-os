#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

char *getcwd(char *buf, size_t size) {
  if (size == 0) {
    errno = EINVAL;
    return NULL;
  }
  vfs_file_t* cwd = _fcntl_get_file(AT_FDCWD);
  errno = vfs.get_path(cwd, buf, size);
  return errno ? 0 : buf;
}
