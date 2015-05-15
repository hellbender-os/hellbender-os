#include <string.h>

int strncmp(const char *str1, const char *str2, size_t num) {
  for (; num && *str1 && *str2 && *str1 == *str2; ++str1, ++str2, --num);
  return (*str1 - *str2);
}
