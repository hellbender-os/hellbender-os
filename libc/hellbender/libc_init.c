#include <kernel/thread.h>
#include <sys/heap.h>

void _hellbender_libc_init() {
  // init the malloc heap.
  heap_init_wilderness(&default_wilderness,
                       (void*)THREAD_HEAP_BOTTOM,
                       (void*)THREAD_HEAP_TOP,
                       HEAP_DEFAULT_ALLOCATION_SIZE);
  heap_init_tiny(&default_tinyheap, &default_wilderness);
  heap_init_small(&default_smallheap, &default_wilderness);
}
