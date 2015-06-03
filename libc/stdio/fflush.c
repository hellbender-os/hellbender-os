#include <stdio.h>
#include <unistd.h>

#include "../fcntl/fcntl_impl.h"

int fflush(FILE *file) {
  if (file) {
    if (file->fd >= 0 && !file->row) {
      write(file->fd, file->buffer, file->buf_bytes);
      file->buf_bytes = 0;
    }
  } else {
    for (file = _LL_FIRST(&_stdio_files, FILE);
         file; file = _LL_NEXT(file, FILE)) {
      fflush(file);
    }
  }
  return 0;
}
