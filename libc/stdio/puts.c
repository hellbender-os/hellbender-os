#include <stdio.h>
#include <string.h>

#if defined(__is_hellbender_kernel)

int puts(const char* string) {
  size_t len = strlen(string);
  if (len == _fprint_n(NULL, string, len)) {
    return putchar('\n');
  }
  else return EOF;
}

#else

int puts(const char* string) {
  size_t len = strlen(string);
  if (len == _fprint_n(stdout, string, len)) {
    return putchar('\n');
  }
  else return EOF;
}

#endif
