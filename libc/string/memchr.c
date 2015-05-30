#include <string.h>

void *memchr(const void *vptr, int ic, size_t size) {
  unsigned char c = (unsigned char)ic;
  unsigned char* ptr = (unsigned char*)vptr;
  for (size_t i = 0; i < size; ++i) {
    if (ptr[i] == c) return ptr;
  }
  return NULL;
}
