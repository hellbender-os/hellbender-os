#include <stdio.h>
#include <unistd.h>

int fflush(FILE *file) {
  if (file) {
    /* TODO implement files
    if (file->fd >= 0 && !file->row) {
      write(file->fd, file->buffer, file->buf_bytes);
      file->buf_bytes = 0;
    }
    */
  } else {
    /* TODO flush all files
    for (file = _LL_FIRST(&_stdio_files, FILE);
         file; file = _LL_NEXT(file, FILE)) {
      fflush(file);
    }
    */
  }
  return 0;
}
