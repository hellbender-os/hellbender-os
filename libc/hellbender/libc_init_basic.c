#include <stdio.h>
#include <stdlib.h>
#include <hellbender.h>

#include <kernel/kernel.h>
#include <kernel/domain.h>

#include <sys/heap.h>

static void init_heap();

/**
 * Initializes all libC features that don't require core services.
 * These feature can be used in libK, and while initializing core services.
 */
void _hellbender_libc_init_basic() {
  // malloc, free, realloc.
  init_heap();
}

static void init_heap() {
  // get the heap address space.
#if defined(__is_hellbender_kernel)
  void* bottom = kernel.heap_bottom;
  void* top = kernel.heap_limit;
#else
  domain_t domain;
  void* bottom = NULL;
  void* top = NULL;
  if (syscall_current_domain(&domain)) {
    bottom = domain.heap_bottom;
    top = domain.heap_limit;
  } else {
    printf("Failed to get current domain information.\n");
    abort();
  }
#endif
  
  // init the malloc heap.
  heap_init_wilderness(&default_wilderness, bottom, top,
                       HEAP_DEFAULT_ALLOCATION_SIZE);
  heap_init_tiny(&default_tinyheap, &default_wilderness);
  heap_init_small(&default_smallheap, &default_wilderness);
}
