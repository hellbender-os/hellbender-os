#ifndef __HELLBENDER_KERNEL_PAGE_H__
#define __HELLBENDER_KERNEL_PAGE_H__

#include "config.h"
#include "kernel.h"

#include <stdint.h>
#include <string.h>

#define PAGE_NOEXECUTE    0x8000000000000000
#define PAGE_GLOBAL       0x0000000000000100
#define PAGE_LARGE        0x0000000000000080
#define PAGE_CACHEDISABLE 0x0000000000000010
#define PAGE_WRITETHROUGH 0x0000000000000008
#define PAGE_USERMODE     0x0000000000000004
#define PAGE_WRITEABLE    0x0000000000000002
#define PAGE_PRESENT      0x0000000000000001
#define PAGE_KERNEL_RW    (1|2)
#define PAGE_KERNEL_RO    (1)
#define PAGE_USER_RW      (1|2|4)
#define PAGE_USER_RO      (1|4)

INLINE void page_invalidate(void *virtual) {
  asm volatile("invlpg (%1)"
               : "=m"(__force_order)
               : "r"((uint64_t)virtual)
               );
}

INLINE void page_invalidate_all() {
  register uint64_t tmp;
  asm volatile("movq %%cr3, %1;"
               "movq %1, %%cr3;"
               : "=m"(__force_order), "=r"(tmp)
               :
               );
}

INLINE uintptr_t page_round_down(uintptr_t physical)  {
  return physical & PAGE_PHYSICAL_MASK;
}

INLINE uintptr_t page_round_up(uintptr_t physical) {
  if (physical % PAGE_SIZE) {
    return physical + (PAGE_SIZE - (physical % PAGE_SIZE));
  } else return physical;
}

INLINE uintptr_t page_clear(uintptr_t physical) {
  memset(kernel_p2v(physical), 0, PAGE_SIZE);
  return physical;
}

void* page_map_4k(void* virtual, uintptr_t physical, uint64_t attributes);
void* page_map_2M(void* virtual, uintptr_t physical, uint64_t attributes);

uintptr_t page_unmap_4k(void* virtual);
uintptr_t page_unmap_2M(void* virtual);

uintptr_t page_remap_4k(void* virtual, uint64_t attributes);
uintptr_t page_remap_2M(void* virtual, uint64_t attributes);

uintptr_t page_set_pdpt(void* virtual, uintptr_t physical, uint64_t attributes);
uintptr_t page_set_pd(void* virtual, uintptr_t physical, uint64_t attributes);

uintptr_t page_get_pdpt(void* virtual);
uintptr_t page_get_address(void* virtual);

#endif
