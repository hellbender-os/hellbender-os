#include <stdio.h>

int ferror(FILE *file) {
  return file->error;
}

