#include <stdlib.h>

void qsort (void* vbase, size_t num, size_t size,
            int (*compare)(const void*,const void*)) {
  char* base = (char*)vbase;
  for (size_t i = 0; i < num; ++i, base = base + size) {
    // find the smallest elelment.
    char* min = base;
    char* ptr = base + size;
    for (size_t j = i + 1; j < num; ++j, ptr = ptr + size) {
      int result = (*compare)(min, ptr);
      if (result > 0) min = ptr;
    }
    // swap so that the smallest is first.
    if (min != base) {
      for (size_t j = 0; j < size; ++j) {
        char tmp = base[j];
        base[j] = min[j];
        min[j] = tmp;
      }
    }
  }
}
