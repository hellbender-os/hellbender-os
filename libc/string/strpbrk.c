#include <string.h>

char *strpbrk(const char *str, const char *chars) {
  for (; *str; ++str) {
    for (const char* ptr = chars; *ptr; ++ptr) {
      if (*str == *ptr) return (char*)str;
    }
  }
  return NULL;
}

