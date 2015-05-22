
#include <kernel/kernel.h>
#include <kernel/mem.h>
#include <sys/heap.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "heap_impl.h"

wilderness_t default_wilderness;
tinyheap_t default_tinyheap;
smallheap_t default_smallheap;

void heap_init_wilderness(wilderness_t *wild,
                          void *start, size_t alloc_size) {
  if (alloc_size % PAGE_SIZE) {
    printf("heap allocation size must be page aligned.\n");
    abort();
  }
  wild->base = (uint8_t*)start;
  wild->bottom = (uint8_t*)start;
  wild->top = (uint8_t*)start;
  wild->step = alloc_size;
  heap_grow_wilderness(wild);
}

void heap_init_tiny(tinyheap_t *heap, wilderness_t *wild) {
  memset(heap, 0, sizeof(tinyheap_t));
  heap->wilderness = wild;
}
 
void heap_init_small(smallheap_t *heap, wilderness_t *wild) {
  memset(heap, 0, sizeof(smallheap_t));
  heap->wilderness = wild;
}
