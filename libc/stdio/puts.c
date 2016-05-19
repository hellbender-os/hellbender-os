#include <stdio.h>
#include <string.h>

int puts(const char* string) {
  size_t len = strlen(string);
#if defined(__is_hellbender_kernel)
  if (len == fwrite(string, len, 1, NULL)) {
#else
  if (len == fwrite(string, len, 1, stdout)) {
#endif
    return putchar('\n');
  }
  else return EOF;
}

