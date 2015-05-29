#include <stdio.h>
#include <unistd.h>

int fflush(FILE *file) {
  if (file->fd >= 0 && !file->row) {
    write(file->fd, file->buffer, file->buf_bytes);
    file->buf_bytes = 0;
  }
  return 0;
}
