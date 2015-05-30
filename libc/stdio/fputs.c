#include <stdio.h>
#include <string.h>

int fputs(const char* string, FILE *file) {
  size_t len = strlen(string);
  if (len == _fprint_n(file, string, len)) {
    return fputc('\n', file);
  } else return EOF;
}
