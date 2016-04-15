#include <string.h>

char *strstr(const char *str1, const char *str2) {
  for (; *str1; ++str1) {
    const char *p1 = str1;
    const char *p2 = str2;
    while (*p1 && *p1 == *p2) ++p1, ++p2;
    if (!*p2) return (char*)str1;
  }
  return NULL;
}

