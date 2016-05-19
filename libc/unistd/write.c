#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

ssize_t write(int fildes, const void *buffer, size_t size) {
  _fd_t *fd = _fcntl_peek_fd(fildes);
  if (!fd || !(fd->file.oflags & O_WRONLY)) {
    errno = EBADF;
    return -1;
  }
  errno = fd->file.op.write(&fd->file, fd->offset, buffer, &size);
  if (!errno) fd->offset += size;
  return errno ? -1 : (ssize_t)size;
}

ssize_t pwrite(int fildes, const void *buffer, size_t size, off_t offset) {
 vfs_file_t *file = _fcntl_get_file(fildes);
  if (!file || !(file->oflags & O_WRONLY)) {
    errno = EBADF;
    return -1;
  }
  errno = file->op.write(file, offset, buffer, &size);
  return errno ? -1 : (ssize_t)size;
}
