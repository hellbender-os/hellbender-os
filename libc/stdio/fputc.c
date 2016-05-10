#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int fputc(int ic, FILE* file) {
  if (fwrite(&ic, 1, 1, file) == 1) {
    return ic;
  } else {
    return EOF;
  }
}
