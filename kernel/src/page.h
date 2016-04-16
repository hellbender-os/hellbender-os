#ifndef __HELLBENDER_KERNEL_PAGE_H__
#define __HELLBENDER_KERNEL_PAGE_H__

#include "config.h"
#include "kernel.h"

#include <stdint.h>
#include <string.h>

#define PAGE_USERMODE 4
#define PAGE_WRITEABLE 2
#define PAGE_PRESENT 1
#define PAGE_KERNEL_RW (1|2)
#define PAGE_KERNEL_RO (1)
#define PAGE_USER_RW (1|2|4)
#define PAGE_USER_RO (1|4)
#define PAGE_LARGE 128

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
  return physical & PAGE_ADDRESS_MASK;
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

void* page_map_4k(void* virtual, uintptr_t physical, unsigned attributes);
void* page_map_2M(void* virtual, uintptr_t physical, unsigned attributes);

uintptr_t page_unmap_4k(void* virtual);
uintptr_t page_unmap_2M(void* virtual);

uintptr_t page_remap_4k(void* virtual, unsigned attributes);
uintptr_t page_remap_2M(void* virtual, unsigned attributes);

uintptr_t page_set_pdpt(void* virtual, uintptr_t physical, unsigned attributes);
uintptr_t page_set_pd(void* virtual, uintptr_t physical, unsigned attributes);

#endif
