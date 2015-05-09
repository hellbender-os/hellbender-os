#include <stdlib.h>
#include <stdio.h>
#include <sys/heap.h>

void* realloc(void* ptr, size_t size) {
  if (ptr == NULL) return malloc(size);
  else {
    if (heap_is_tiny_ptr(ptr)) {
      return heap_realloc_tiny(&default_tinyheap, ptr, size);
    } else {
      return heap_realloc_small(&default_smallheap, ptr, size);
    }
  }
}
