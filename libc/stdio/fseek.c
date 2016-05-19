#include <stdio.h>
#include <unistd.h>

int fseek(FILE *file, long off, int where) {
  if (lseek(file->fd, off, where) >= 0) return 0;
  else return 1;
}

int fseeko(FILE *file, off_t off, int where) {
  if (lseek(file->fd, off, where) >= 0) return 0;
  else return 1;
}

