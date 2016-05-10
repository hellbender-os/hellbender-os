#include <stdio.h>
#include <string.h>

int fputs(const char* string, FILE *file) {
  size_t len = strlen(string);
  if (len == fwrite(string, len, 1, file)) {
    return 0;
  } else return EOF;
}
