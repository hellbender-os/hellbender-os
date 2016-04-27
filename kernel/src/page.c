#include "page.h"
#include "kernel.h"
#include "lomem.h"

// page directory for the first 1 Gb.
uint64_t page_dir[512] __attribute__((aligned(PAGE_SIZE)));
// first 1GB mapped using 2MB pages.

// page directory pointer table for the first 512 Gb.
uint64_t page_dpt[512] __attribute__((aligned(PAGE_SIZE)));
/*  = { (uint64_t)&page_dir | 3 }; */

// page-map level-4 table
uint64_t page_ml4t[512] __attribute__((aligned(PAGE_SIZE)));
/* = { 0 (uint64_t)&pdpt | 3 }; */

INLINE void page_ensure_pdp_table(void* virtual, uint64_t attributes) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pdpt = (uint64_t *)(((address>>36) & ~7) | 0xFFFFFFFFFFFFF000);
  if (!*pdpt) *pdpt = page_clear(lomem_alloc_4k()) | attributes;
}

INLINE void page_ensure_directory(void* virtual, uint64_t attributes) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pd = (uint64_t *)(((address>>27) & ~7) | 0xFFFFFFFFFFE00000);
  if (!*pd) *pd = page_clear(lomem_alloc_4k()) | attributes;
}

INLINE void page_ensure_pagetable(void* virtual, uint64_t attributes) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pt = (uint64_t *)(((address>>18) & ~7) | 0xFFFFFFFFC0000000);
  if (!*pt) *pt = page_clear(lomem_alloc_4k()) | attributes;
}

void* page_map_4k(void* virtual, uintptr_t physical, uint64_t attributes) {
  page_ensure_pdp_table(virtual, attributes);
  page_ensure_directory(virtual, attributes);
  page_ensure_pagetable(virtual, attributes);
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *page = (uint64_t *)(((address>> 9) & ~7) | 0xFFFFFF8000000000);
  if (!*page) *page = (physical & PAGE_PHYSICAL_MASK) | attributes;
  else kernel_panic(); // overwriting a page is not allowed!
  return (void*)(((uintptr_t)virtual) & PAGE_PHYSICAL_MASK);
}

void* page_map_2M(void* virtual, uintptr_t physical, uint64_t attributes) {
  page_ensure_pdp_table(virtual, attributes);
  page_ensure_directory(virtual, attributes);
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pt = (uint64_t *)(((address>>18) & ~7) | 0xFFFFFFFFC0000000);
  if (!*pt) *pt = (physical & TABLE_PHYSICAL_MASK) | attributes | PAGE_LARGE;
  else kernel_panic(); // overwriting a page table is not allowed!
  return (void*)(((uintptr_t)virtual) & TABLE_PHYSICAL_MASK);
}

uintptr_t page_unmap_4k(void* virtual) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *page = (uint64_t *)(((address>> 9) & ~7) | 0xFFFFFF8000000000);
  if (*page & PAGE_PRESENT) {
    uintptr_t physical = *page & PAGE_PHYSICAL_MASK;
    *page = 0;
    return physical;
  } else kernel_panic(); // unmapping non-existing page.
}

uintptr_t page_unmap_2M(void* virtual) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pt = (uint64_t *)(((address>>18) & ~7) | 0xFFFFFFFFC0000000);
  if (*pt & PAGE_PRESENT) {
    uintptr_t physical = *pt & TABLE_PHYSICAL_MASK;
    *pt = 0;
    return physical;
  } else kernel_panic(); // unmapping non-existing page.
}

uintptr_t page_remap_4k(void* virtual, uint64_t attributes) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *page = (uint64_t *)(((address>> 9) & ~7) | 0xFFFFFF8000000000);
  uintptr_t physical = *page & PAGE_PHYSICAL_MASK;
  *page = physical | attributes;
  return physical;
}

uintptr_t page_remap_2M(void* virtual, uint64_t attributes) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pt = (uint64_t *)(((address>>18) & ~7) | 0xFFFFFFFFC0000000);
  uintptr_t physical = *pt & PAGE_PHYSICAL_MASK;
  *pt = physical | attributes;
  return physical;
}

uintptr_t page_set_pdpt(void* virtual, uintptr_t physical, uint64_t attributes) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pdpt = (uint64_t *)(((address>>36) & ~7) | 0xFFFFFFFFFFFFF000);
  uintptr_t old = *pdpt;
  *pdpt = physical | attributes;
  return old;
}

uintptr_t page_set_pd(void* virtual, uintptr_t physical, uint64_t attributes) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pd = (uint64_t *)(((address>>27) & ~7) | 0xFFFFFFFFFFE00000);
  uintptr_t old = *pd;
  *pd = physical | attributes;
  return old;
}

uintptr_t page_get_address(void* virtual) {
  uintptr_t address = (uintptr_t)virtual;
  uint64_t *pt = (uint64_t *)(((address>>18) & ~7) | 0xFFFFFFFFC0000000);
  uint64_t table = *pt;
  if (table & PAGE_LARGE) { // this is 2MB page
    return (table & TABLE_PHYSICAL_MASK) | (((uint64_t)virtual) & TABLE_OFFSET_MASK);
  } else { // this is 4k page
    uint64_t *page = (uint64_t *)(((address>> 9) & ~7) | 0xFFFFFF8000000000);
    return *page & PAGE_PHYSICAL_MASK;
  }
}
