#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

ssize_t read(int fildes, void *buffer, size_t size) {
  _fd_t *fd = _fcntl_peek_fd(fildes);
  if (!fd || !(fd->file.oflags & O_RDONLY)) {
    errno = EBADF;
    return -1;
  }
  errno = fd->file.op.read(&fd->file, fd->offset, buffer, &size);
  return errno ? -1 : (ssize_t)size;
}

ssize_t pread(int fildes, void *buffer, size_t size, off_t offset) {
  vfs_file_t *file = _fcntl_get_file(fildes);
  if (!file || !(file->oflags & O_RDONLY)) {
    errno = EBADF;
    return -1;
  }
  errno = file->op.read(file, offset, buffer, &size);
  return errno ? -1 : (ssize_t)size;
}
