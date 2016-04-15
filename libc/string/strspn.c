#include <string.h>

size_t strspn(const char *str1, const char *str2) {
  size_t i = 0;
  for (; str1[i]; ++i) {
    if (!strchr(str2, str1[i])) break;
  }
  return i;
}
