#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "heap.h"

void* malloc(size_t size) {
  return _heap_alloc(size);
}
