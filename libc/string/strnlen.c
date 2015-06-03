#include <string.h>

size_t strnlen(const char *str, size_t maxlen) {
  for (size_t i = 0; i < maxlen; ++i) {
    if (!str[i]) return i;
  }
  return maxlen;
}
