#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

int fcntl(int fildes, int cmd, ...) {
  _fd_t *fd = _fcntl_peek_fd(fildes);
  if (!fd) {
    errno = EINVAL;
    return -1;
  }
  if (!fd->file.oflags) {
    errno = EBADF;
    return -1;
  }
  
  switch (cmd) {
  case F_DUPFD:
  case F_DUPFD_CLOEXEC:
    {
      va_list parameters;
      va_start(parameters, cmd);
      int arg = va_arg(parameters, int);
      va_end(parameters);
      int fildes2 = _fcntl_reserve_fildes(arg);
      if (fildes2 < 0) {
        errno = EMFILE;
        return -1;
      }
      if (dup2(fildes, fildes2)) {
        _fcntl_release_fildes(fildes2);
        return -1;
      }
      if (cmd == F_DUPFD_CLOEXEC) {
        _fcntl_get_fd(fildes2)->flags |= FD_CLOEXEC;
      }
      return fildes2;
    }

  case F_GETFD:
    return fd->flags;

  case F_SETFD:
    {
      va_list parameters;
      va_start(parameters, cmd);
      int arg = va_arg(parameters, int);
      va_end(parameters);
      fd->flags = arg;
      return 0;
    }

  case F_GETFL:
    return fd->file.oflags;
    
  case F_SETFL:
    {
      va_list parameters;
      va_start(parameters, cmd);
      int arg = va_arg(parameters, int);
      va_end(parameters);
      fd->file.oflags = (fd->file.oflags & O_ACCMODE) | (arg & ~O_ACCMODE);
      return 0;
    }
    
  case F_GETOWN:
    errno = ENOTSUP;
    return -1; //TODO
    
  case F_SETOWN:
    errno = ENOTSUP;
    return -1; //TODO
    /*{
      va_list parameters;
      va_start(parameters, cmd);
      int arg = va_arg(parameters, int);
      va_end(parameters);
      }*/
    
  case F_GETLK:
  case F_SETLK:
  case F_SETLKW:
    // TODO: support advisary locks.
    errno = ENOTSUP;
    return -1;
    
  default:
    errno = EINVAL;
    return -1;
  }
}
