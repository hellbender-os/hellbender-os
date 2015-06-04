#include <hellbender/heap.h>
#include <hellbender/hellbender.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <kernel/domain.h>

#if defined(__is_hellbender_kernel)

static inline void* do_allocate(void* address, size_t size) {
  void* end_ptr = domain_set_break(&kernel_domain, address + size, 0);
  if (end_ptr == (void*)-1) {
    printf("Out of memory.\n");
    abort();
  }
  //printf("Reserved heap space: %x - %x\n",
  //       (unsigned)address, (unsigned)end_ptr);
  return (void*)end_ptr;
}

#elif defined(__is_hellbender_libc)

static inline void* do_allocate(void* address, size_t size) {
  void* end_ptr = syscall_set_program_break(address + size, 0);
  if (end_ptr == (void*)-1) {
    printf("Out of memory.\n");
    abort();
  }
  //printf("Reserved heap space: %x - %x\n",
  //       (unsigned)address, (unsigned)end_ptr);
  return (void*)end_ptr;
}

#else

static inline void* do_allocate(void* address, size_t size) {
  // this is our test-case code! we assume that the actual memory exists.
  uintptr_t end_ptr = (uintptr_t)address + size;
  memset(address, 0, size);  
  return (void*)end_ptr;
}

#endif

void heap_grow_wilderness(wilderness_t *wild) {
  uint8_t* new_top = wild->top + wild->step;
  do_allocate(wild->top, wild->step);
  wild->top = new_top;
}
