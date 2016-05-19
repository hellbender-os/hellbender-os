#include <stdio.h>

void clearerr(FILE *file) {
  file->error = 0;
}

