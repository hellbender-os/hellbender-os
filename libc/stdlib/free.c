#include <stdlib.h>
#include <stdio.h>
#include <sys/heap.h>

void free(void* ptr) {
  if (heap_is_tiny_ptr(ptr)) {
    heap_free_tiny(&default_tinyheap, ptr);
  } else {
    heap_free_small(&default_smallheap, ptr);
  }
}
