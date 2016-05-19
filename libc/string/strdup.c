#include <string.h>
#include <stdlib.h>

char *strdup(const char *str) {
  size_t len = strlen(str) + 1;
  char *dup = (char*)malloc(len);
  memcpy(dup, str, len);
  return dup;
}

