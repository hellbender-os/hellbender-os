#include <stdio.h>
#include <unistd.h>

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *file) {
  size_t data_length = size * count;
  if (file->buffer == NULL && file->fd >= 0) { // unbuffered
    /* TODO implement files
    write(file->fd, ptr, data_length);
    */
  } else { // buffered
    char *data = (char*)ptr;
    for (size_t i = 0; i < data_length; i++) {
      if (file->buf_bytes == file->buf_size) fflush(file);
      if (file->buf_bytes < file->buf_size)
        file->buffer[(file->buf_bytes)++] = data[i];
      if (data[i] == '\n') fflush(file);
    }
  }
  return data_length;
}
