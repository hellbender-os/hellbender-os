#include <string.h>

char *stpcpy(char *dest, const char *src) {
  unsigned i = 0;
  for (; src[i]; ++i) dest[i] = src[i];
  dest[i] = 0;
  return dest + i;
}
