#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "heap.h"

void* malloc(size_t size) {
  if (size < HEAP_ALLOC_LIMIT) return _heap_alloc(size);
  else {
    void* ptr = mmap(0, size+8, PROT_READ|PROT_WRITE, MAP_ANONYMOUS, -1, 0);
    uint64_t *hrd = (uint64_t*)ptr;
    hrd[-1] = size;
    return ptr;
  }
}
