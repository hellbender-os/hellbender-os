#include <stdio.h>
#include <unistd.h>

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *file) {
  size_t bytes = size * count;
  return write(file->fd, buffer, bytes);
}
