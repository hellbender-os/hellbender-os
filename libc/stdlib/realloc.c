#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

void* realloc(void* ptr, size_t size) {
  if (ptr == NULL) return malloc(size);
  else if (size == 0) {
    free(ptr);
    return NULL;
  } else {
    return _heap_realloc(ptr, size);
  }
}
