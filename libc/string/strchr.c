#include <string.h>

char *strchr(const char *str, int c) {
  while (*str && *str != c) ++str;
  if (!*str && c) return NULL;
  else return (char*)str;
}

