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
 *
 * Basic init for libK cannot use syscalls, it has to do direct access.
 */
void _hellbender_libc_init_basic() {
  // malloc, free, realloc.
  init_heap();
}

static void init_heap() {
  // get the heap address space.
#if defined(__is_hellbender_kernel)
  void* bottom = (uintptr_t)domain_set_break(&kernel_domain, NULL, 0);
#else
  void* bottom = syscall_set_program_break(NULL, 0);
#endif
 
  // init the malloc heap.
  heap_init_wilderness(&default_wilderness, bottom, 
                       HEAP_DEFAULT_ALLOCATION_SIZE);
  heap_init_tiny(&default_tinyheap, &default_wilderness);
  heap_init_small(&default_smallheap, &default_wilderness);
}
