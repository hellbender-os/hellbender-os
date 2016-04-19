#include <string.h>

char *strrchr(const char *str, int ch) {
  char *ptr = NULL;
  for (; *str; ++str) if (*str == ch) ptr = (char*)str;
  return ptr;
}

