#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "heap.h"

void free(void *ptr) {
    uint64_t *hrd = (uint64_t*)ptr;
    uint64_t size = hrd[-1];
    if (size < HEAP_ALLOC_LIMIT) _heap_free(ptr);
    else munmap(hrd - 1, size + 8);
}
