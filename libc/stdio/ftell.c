#include <stdio.h>
#include <unistd.h>

long ftell(FILE *file) {
  return lseek(file->fd, 0, SEEK_CUR);
}

off_t ftello(FILE *file) {
  return (off_t) lseek(file->fd, 0, SEEK_CUR);
}

