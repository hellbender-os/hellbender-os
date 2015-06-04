#include <stdlib.h>
#include <stdio.h>
#include <hellbender/heap.h>

void* realloc(void* ptr, size_t size) {
  if (ptr == NULL) return malloc(size);
  else if (size == 0) {
    free(ptr);
    return NULL;
  } else {
    if (heap_is_tiny_ptr(ptr)) {
      return heap_realloc_tiny(&default_tinyheap, ptr, size);
    } else if (heap_is_small_ptr(ptr)) {
      return heap_realloc_small(&default_smallheap, ptr, size);
    } else {
      return heap_realloc_large(&default_largeheap, ptr, size);
    }
  }
}
