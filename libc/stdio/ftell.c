#include <stdio.h>
#include <unistd.h>

long ftell(FILE *file) {
  return lseek(file->fd, 0, SEEK_CUR);
}
