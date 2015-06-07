#include <unistd.h>
#include <errno.h>

#include "../fcntl/fcntl_impl.h"

void sync(void) {
  for (int i = 0; i < OPEN_MAX; ++i) {
    struct vfs_file *file = &_fcntl_data.handles[i];
    if (file->stat.st_mode && file->filesys.fsync) {
      IDC(vfs_fsync, file->filesys.fsync, file);
    }
  }
}

int fsync(int fildes) {
  if (fildes < 0 || fildes >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[fildes];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.fsync) {
    errno = EINVAL;
    return -1;
  }
  return IDC(vfs_fsync, file->filesys.fsync, file);
}

int fdatasync(int fildes) {
  return fsync(fildes);
}
