#include <string.h>

int strcmp(const char *str1, const char *str2) {
  for (; *str1 && *str2 && *str1 == *str2; ++str1, ++str2);
  return (*str1 - *str2);
}
