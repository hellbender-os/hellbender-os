#include <stdio.h>

void setbuf(FILE *file, char *buf) {
  fflush(file);
  file->buffer = buf;
}
