#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

int dup(int fildes) {
  return fcntl(fildes, F_DUPFD, 0);
}

int dup2(int old_fd, int new_fd) {
  _fd_t *fd1 = _fcntl_peek_fd(old_fd);
  if (!fd1 || !fd1->file.oflags) {
    errno = EBADF;
    return -1;
  }
  if (new_fd < 0 || new_fd >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  if (old_fd == new_fd) return new_fd;
  _fd_t *fd2 = _fcntl_get_fd(new_fd);
  if (fd2->file.oflags) {
    errno = fd2->file.op.close(&fd2->file);
    if (errno) return -1;
  }
  fd1->file.op.dup(&fd1->file, &fd2->file);
  fd2->offset = fd1->offset;
  fd2->flags = fd1->flags & ~FD_CLOEXEC;
  return new_fd;
}
