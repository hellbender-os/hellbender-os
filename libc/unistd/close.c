#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

int close(int fd) {
  vfs_file_t *file = _fcntl_get_file(fd);
  if (!file || !(file->oflags)) {
    errno = EBADF;
    return -1;
  }
  errno = file->op.close(file);
  if (!errno) {
    _fcntl_release_fildes(fd);
  }
  return errno ? -1 : 0;
}

