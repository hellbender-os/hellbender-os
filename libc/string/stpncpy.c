#include <string.h>

char *stpncpy(char *dest, const char *src, size_t n) {
  unsigned i = 0;
  for (; src[i] && i < n; ++i) dest[i] = src[i];
  char *retval = dest + i;
  for (; i < n; ++i) dest[i] = 0;
  return retval;
}
