#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include "fcntl_impl.h"

int fcntl(int fildes, int cmd, ...) {
  if (fildes < 0 || fildes >= OPEN_MAX) {
    errno = EINVAL;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[fildes];
  if (!file->stat.st_mode) {
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
      int handle = fcntl_allocate_handle_ge(arg);
      if (handle < 0) {
        errno = EMFILE;
        return -1;
      }
      struct vfs_file *dst = &_fcntl_data.handles[handle];
      memcpy(dst, file, sizeof(struct vfs_file));
      if (cmd == F_DUPFD) {
        file->oflags &= ~FD_CLOEXEC;
      } else {
        file->oflags |= FD_CLOEXEC;
      }
      return handle;
    }

  case F_GETFD:
    return file->oflags & FD_CLOEXEC;

  case F_SETFD:
    {
      va_list parameters;
      va_start(parameters, cmd);
      int arg = va_arg(parameters, int);
      va_end(parameters);
      file->oflags = (file->oflags & ~FD_CLOEXEC) | (arg & FD_CLOEXEC);
      return 0;
    }

  case F_GETFL:
    return file->oflags & (_O_STATFLAGS | O_ACCMODE);
    
  case F_SETFL:
    {
      va_list parameters;
      va_start(parameters, cmd);
      int arg = va_arg(parameters, int);
      va_end(parameters);
      file->oflags = (file->oflags & ~_O_STATFLAGS) | (arg & _O_STATFLAGS);
      return 0;
    }
    
  case F_GETOWN:
    return (int)file->owner;
    
  case F_SETOWN:
    {
      va_list parameters;
      va_start(parameters, cmd);
      int arg = va_arg(parameters, int);
      va_end(parameters);
      file->owner = (id_t)arg;
      return 0;
    }
    
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
