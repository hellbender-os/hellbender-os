#include <stdio.h>

int fgetc(FILE *file) {
  unsigned char c;
  if (fread(&c, sizeof(c), 1, file) == sizeof(c)) {
    return c;
  } else {
    return EOF;
  }
}
