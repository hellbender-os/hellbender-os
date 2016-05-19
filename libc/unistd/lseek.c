#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

off_t lseek(int fildes, off_t off, int where) {
  _fd_t *fd = _fcntl_peek_fd(fildes);
  if (!fd || !fd->file.oflags) {
    errno = EBADF;
    return -1;
  }
  switch (where) {
  case SEEK_SET:
    if ((fd->offset = off) < 0) fd->offset = 0;
    break;
  case SEEK_CUR:
    if ((fd->offset += off) < 0) fd->offset = 0;
    break;
  case SEEK_END:
    {
      struct stat stat;
      fd->file.op.stat(&fd->file, &stat);
      if ((fd->offset = stat.st_size + off) < 0) fd->offset = 0;
    }
    break;
  default:
    errno = EINVAL;
    return -1;
  }
  return fd->offset;
}
