#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <kernel/kernel.h>

#include "fcntl_impl.h"

int open(const char *name, int flags, ...) {
  if (flags & O_CREAT) {
    va_list parameters;
    va_start(parameters, flags);
    mode_t mode = va_arg(parameters, mode_t);
    return openat(AT_FDCWD, name, flags, mode);
  } else {
    return openat(AT_FDCWD, name, flags);
  }
}
