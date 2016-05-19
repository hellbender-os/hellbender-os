#include <string.h>
#include <stdlib.h>

char *strndup(const char *str, size_t n) {
  size_t len = strlen(str);
  if (len > n) len = n;
  char *ptr = (char*)malloc(len+1);
  memcpy(ptr, str, len);
  ptr[len] = 0;
  return ptr;
}
