#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

int open(const char *name, int flags, ...) {
  int fd = -1;
  if (name[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  if (flags & O_CREAT) {
    va_list parameters;
    va_start(parameters, flags);
    mode_t mode = va_arg(parameters, mode_t);
    return openat(fd, name, flags, mode);
  } else {
    return openat(fd, name, flags);
  }
}

int openat(int dir_fd, const char *name, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list parameters;
    va_start(parameters, flags);
    mode = va_arg(parameters, mode_t);
  }

  // check dir_fd
  vfs_file_t* dir = NULL;
  if (name[0] != VFS_SEPARATOR_CHAR) {
    dir = _fcntl_get_file(dir_fd);
    if (!dir) {
      errno = EBADF;
      return -1;
    }
  }

  // check file length.
  if (strlen(name) == 0) {
    errno = ENOENT;
    return -1;
  }
  
  // allocate virtual file object.
  int fildes = _fcntl_reserve_fildes(0);
  if (fildes < 0) {
    errno = EMFILE;
    return -1;
  }

  // resolve the path.
  _fd_t *fd = _fcntl_get_fd(fildes);
  errno = vfs.open(dir, name, flags, 0, mode & _fcntl.umask, &fd->file);
  if (errno) {
    _fcntl_release_fildes(fildes);
    fildes = -1;
  }
  return fildes;
}
