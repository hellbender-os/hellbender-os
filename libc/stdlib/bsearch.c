#include <stdlib.h>

void* bsearch (const void* key, const void* vbase,
               size_t num, size_t size,
               int (*compare)(const void*,const void*)) {
  char* base = (char*)vbase;
  for (size_t i = 0; i < num; ++i, base = base + size) {
    int result = (*compare)(key, base);
    if (result == 0) return base;
  }
  return NULL;
}
