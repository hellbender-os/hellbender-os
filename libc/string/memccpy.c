#include <string.h>

void *memccpy(void *s1, const void *s2, int c, size_t n) {
  unsigned char *ptr1 = (unsigned char*)s1;
  unsigned char *ptr2 = (unsigned char*)s2;
  for (size_t i = 0; i < n; ++i) {
    ptr1[i] = ptr2[i];
    if (ptr1[i] == c) return &ptr1[i];
  }
  return NULL;
}
