#include <string.h>

char *strncat(char *dst, const char *src, size_t num) {
  size_t i = 0;
  for (; src[i] && i < num ; ++i) {
    dst[i] = src[i];
  }
  dst[i] = 0;
  return dst;
}
