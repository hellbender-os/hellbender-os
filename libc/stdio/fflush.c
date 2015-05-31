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
    // TODO: flush all
  }
  return 0;
}
