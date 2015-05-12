#include <sys/heap.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if defined(__is_hellbender_kernel)
#include <kernel/mem.h>
#include <kernel/mmap.h>

static inline void* do_allocate(void* address, size_t size) {
  // kernel can directly map stuff.
  // TODO: check virtual address space limits!
  uintptr_t end_ptr = (uintptr_t)address + size;
  mem_alloc_mapped(address, size, MMAP_ATTRIB_USER_RW);
  return (void*)end_ptr;
}

#elif defined(__is_hellbender_libc)
#include <hellbender.h>

static inline void* do_allocate(void* address, size_t size) {
  // libc must use syscall to allocate and map pages.
  uintptr_t end_ptr = (uintptr_t)address + size;
  printf("Reserved heap space: %x - %x\n", (unsigned)address, (unsigned)end_ptr);
  syscall_allocate(address, size);
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
  if (new_top > wild->limit) {
    printf("Out of heap space!\n");
    abort();
  }
  do_allocate(wild->top, wild->step);
  wild->top = new_top;
}
