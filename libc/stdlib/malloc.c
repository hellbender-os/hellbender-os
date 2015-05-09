#include <stdlib.h>
#include <stdio.h>
#include <sys/heap.h>

void* malloc(size_t size) {
  if (size <= TINYHEAP_ALLOC_LIMIT) {
    return heap_malloc_tiny(&default_tinyheap, size);
  } else if (size <= SMALLHEAP_ALLOC_LIMIT) {
    return heap_malloc_small(&default_smallheap, size);
  } else {
    printf("Too large allocation!\n");
    abort();
  }
  return 0;
}
