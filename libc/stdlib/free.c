#include <stdlib.h>
#include <stdio.h>
#include <hellbender/heap.h>

void free(void* ptr) {
  if (!ptr) return;
  if (heap_is_tiny_ptr(ptr)) {
    heap_free_tiny(&default_tinyheap, ptr);
  } else if (heap_is_small_ptr(ptr)) {
    heap_free_small(&default_smallheap, ptr);
  } else {
    heap_free_large(&default_largeheap, ptr);
  }
}
