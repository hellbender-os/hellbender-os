#include <stdio.h>
#include <stdlib.h>

#if defined(__is_hellbender_kernel)
#include <kernel/kernel.h>
#include <kernel/process.h>
#else
#include <sys/keymap.h>
void fcntl_init();
void stdio_init();
keymap_t* keymap;
#endif

#include <kernel/domain.h>
#include <sys/heap.h>
#include <hellbender.h>

void _hellbender_libc_init() {
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
  int is_core = (uintptr_t)domain.domain_base == CORE_OFFSET;
  //if (is_core) {
  //  printf("We are in core srv, some features disabled.\n");
  //} else {
  //  printf("ALL features ENABLED.\n");
  //}
#endif
  
  // init the malloc heap.
  heap_init_wilderness(&default_wilderness, bottom, top,
                       HEAP_DEFAULT_ALLOCATION_SIZE);
  heap_init_tiny(&default_tinyheap, &default_wilderness);
  heap_init_small(&default_smallheap, &default_wilderness);

#if defined(__is_hellbender_kernel)
  // kernel doesn't use all features.
  
#else
  if (!is_core) {
    // file handles.
    fcntl_init();
    stdio_init();
  }

  // TODO: support actual locales.
  keymap = keymap_create("fi");
#endif

}
