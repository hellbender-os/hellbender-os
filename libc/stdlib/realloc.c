#include <stdlib.h>
#include <stdio.h>
#include <sys/heap.h>

void* realloc(void* ptr, size_t size) {
  //return heap_realloc(&heap_default, ptr, size);
  return 0;
}
