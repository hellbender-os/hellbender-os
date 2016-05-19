#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

int truncate(const char *path, off_t length) {
  if (length < 0) {
    errno = EINVAL;
    return -1;
  }
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  vfs_file_t file;
  errno = vfs.open(0, path, O_WRONLY, 0, 0, &file);
  if (errno) return -1;
  errno = file.op.truncate(&file, length);
  file.op.close(&file);
  return errno ? -1 : 0;
}

int ftruncate(int fildes, off_t length) {
  if (length < 0) {
    errno = EINVAL;
    return -1;
  }
  vfs_file_t *file = _fcntl_get_file(fildes);
  if (!file || !(file->oflags & O_WRONLY)) {
    errno = EBADF;
    return -1;
  }
  errno = file->op.truncate(file, length);
  return errno ? -1 : 0;
}
