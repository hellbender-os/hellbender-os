#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <kernel/kernel.h>

#include "fcntl_impl.h"

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
