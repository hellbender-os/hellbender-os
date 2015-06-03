#include <string.h>

char *strncpy(char *dest, const char *src, size_t num) {
  size_t i = 0;
  for (; src[i] && i < num; ++i) {
    dest[i] = src[i];
  }
  for (; i < num; ++i) {
    dest[i] = 0;
  }
  return dest;
}
