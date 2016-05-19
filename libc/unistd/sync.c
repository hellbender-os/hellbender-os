#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

void sync(void) {
  for (int i = 0; i < OPEN_MAX; ++i) {
    vfs_file_t* file = _fcntl_get_file(i);
    if (file && file->oflags & O_WRONLY) {
      file->op.sync(file);
    }
  }
}

int fsync(int fildes) {
  vfs_file_t *file = _fcntl_get_file(fildes);
  if (!file || !(file->oflags & O_WRONLY)) {
    errno = EBADF;
    return -1;
  }
  errno = file->op.sync(file);
  return errno ? -1 : 0;
}

int fdatasync(int fildes) {
  return fsync(fildes);
}
