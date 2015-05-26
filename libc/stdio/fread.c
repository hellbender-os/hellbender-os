#include <stdio.h>
#include <unistd.h>

size_t fread(void *buffer, size_t e_size, size_t nof_e, FILE *file) {
  size_t bytes = e_size * nof_e;
  return read(file->fd, buffer, bytes);  
}
