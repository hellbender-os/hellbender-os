#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/mmap.h>
#include <kernel/mem.h>
#include <kernel/gdt.h>
#include <kernel/tss.h>

typedef struct mmap {
  // a range of free pages that we are consuming next.
  uintptr_t free_pages;
  size_t free_size;
  
  uint32_t *page_directory; // virtual address to the recursive page directory.
  uint32_t *page_tables; // virtual address to the recursive page tables.
} mmap_t;
mmap_t mmap;

uint8_t mmap_page_directory[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mmap_page_table_ds[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mmap_page_table_cs[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mmap_tmp_page[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

extern unsigned long __force_order;
static inline void invalidate(void *virtual) {
  asm volatile("invlpg (%1)"
               : "=m"(__force_order)
               : "r"((uint32_t)virtual)
               );
}

static inline void invalidate_all() {
  asm volatile("mov %%cr3, %%eax;"
               "mov %%eax, %%cr3;"
               : "=m"(__force_order)
               :
               : "%eax"
               );
}
/*
static inline unsigned set_segments() {
  unsigned old_ds;
  asm volatile("mov %%ss, %0;"
               "mov %2, %%ds;"
               "mov %2, %%es;"
               "mov %2, %%ss;"
               : "=&r"(old_ds), "=m"(__force_order)
               : "r"(SEL_ALL_DATA)
               : "memory"
               );
  return old_ds;
}

static inline void reset_segments(unsigned old_ds) {
  asm volatile("mov %1, %%ds;"
               "mov %1, %%es;"
               "mov %1, %%ss;"
               : "=m"(__force_order)
               : "r"(old_ds)
               : "memory"
               );
}
*/
void mmap_stage_2_init() {
  memset(&mmap, 0, sizeof(mmap));
  memset(mmap_page_directory, 0, PAGE_SIZE);
  memset(mmap_page_table_ds, 0, PAGE_SIZE);
  memset(mmap_page_table_cs, 0, PAGE_SIZE);

  uint32_t *page_dir = (uint32_t*)mmap_page_directory;

  // map the first two page tables for DS and CS segments.
  page_dir[0] = ((uint32_t)(mmap_page_table_ds)) | MMAP_ATTRIB_KERNEL_RW;
  page_dir[CS_BASE >> 22] =
    ((uint32_t)(mmap_page_table_cs)) | MMAP_ATTRIB_KERNEL_RW;
  
  // recursive page directory is mapped to the last 4MB.
  // note that access requires SEL_ALL_DATA selector.
  page_dir[1023] = ((uint32_t)page_dir) | MMAP_ATTRIB_KERNEL_RW;
  mmap.page_tables = (uint32_t*)((uintptr_t)1023 * (uintptr_t)0x400000);
  mmap.page_directory = (uint32_t*)((uintptr_t)(mmap.page_tables)
                                    + (uintptr_t)1023 * (uintptr_t)0x1000);
}

void* mmap_stage_2_map_page(void* virtual, uintptr_t page, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  // page directory and page tables are mapped as part of the kernel.
  if ((address % PAGE_SIZE) || (page % PAGE_SIZE)) {
    printf("Address and page must be page aligned!\n");
    abort();
  }
  uint32_t *page_dir = (uint32_t*)mmap_page_directory;
  uint32_t page_dir_index = address >> 22;
  uint32_t page_table_index = (address >> 12) & 0x3ff;
  if ((page_dir[page_dir_index] & 1) == 0) {
    printf("mmap_stage_2_map_page supports only the first 4MB region!\n");
    printf("requested page %x at address %x\n",
           (unsigned)page, (unsigned)virtual);
    abort();
  }
  uint32_t *page_table = (uint32_t*)(page_dir[page_dir_index] & 0xfffff000);
  page_table[page_table_index] = (page & 0xfffff000) | attributes;
  invalidate(virtual);
  return virtual;
}

void* mmap_stage_2_map(void* virtual, uintptr_t physical,
                       size_t size, unsigned attributes) {
  //printf("mapping %x-%x into %x-%x\n",
  //       (unsigned)physical, (unsigned)(physical+size),
  //       (unsigned)virtual, (unsigned)(virtual+size));
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  if (size > TABLE_SIZE) {
    printf("too big.\n");
    abort();
  }
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    printf("Virtual and physical addresses must be page aligned!\n");
    abort();
  }
  for (uint32_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_stage_2_map_page(virtual + i, physical + i, attributes);
  }
  return virtual;
}

void mmap_stage_2_enable_paging() {
#ifdef DEBUG
  printf("mmap_early_enable_paging\n");
#endif
  
  asm ("mov %%eax, %%cr3;"
       "mov %%cr0, %%eax;"
       "or $0x80000000, %%eax;"
       "mov %%eax, %%cr0;"
       : /* no output registers */
       : "a"(&mmap_page_directory)
       ); 
}

void mmap_stage_2_cleanup() {
  // hide page tables to avoid accidentally overwriting them.
  // special selector is used to write into the recursive page directory.
  mmap_unmap_page(&mmap_page_directory);
  mmap_unmap_page(&mmap_page_table_ds);
  mmap_unmap_page(&mmap_page_table_cs);
  mmap_unmap_page(CS_BASE + &mmap_page_directory);
  mmap_unmap_page(CS_BASE + &mmap_page_table_ds);
  mmap_unmap_page(CS_BASE + &mmap_page_table_cs);

  // we just want the virtual memory of tmp_page.
  mem_free_page(mmap_unmap_page(mmap_tmp_page));
}

void* mmap_map_page(void* virtual, uintptr_t physical, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  //printf("Mapping virtual address = %x\n", (unsigned)virtual);
  //kprintf("PDI = %u; PTI = %u; attr = %u\n",
  //        (unsigned)page_dir_index, (unsigned)page_table_index,
  //        (unsigned)attributes);

  uint32_t* page_dir = mmap.page_directory;
  uint32_t* page_tables = mmap.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;
  
  // ensure that the page table exists; set the page entry.
  unsigned value;
  asm volatile("push %%ds;"
               "mov %1, %%ds;"
               "mov %%ds:0(%2), %0;"
               "test $1, %0;"
               "jz 1f;"
               "mov %3, %%ds:0(%4);"
               "1:"
               "pop %%ds;"
               : "=&r"(value)
               : "r"(SEL_ALL_DATA),
                 "r"(page_dir + page_dir_index),
                 "r"((physical & 0xfffff000) | attributes),
                 "r"(page_table + page_table_index)
               : "cc");
  if ((value & 1) == 0) {
    // TODO: we can skip this test; then it'd create a page fault instead.
    printf("Trying to map %x to unmapped page table!\n", (unsigned)virtual);
    abort();
  }

  invalidate(virtual);
  return virtual;
}

uintptr_t mmap_unmap_page(void* virtual) {
  //kprintf("Unmapping virtual address = %x\n", (unsigned)virtual);
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  uint32_t* page_dir = mmap.page_directory;
  uint32_t* page_tables = mmap.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;

  // ensure that the page table exists; clean the page entry.
  unsigned value;
  asm volatile("push %%ds;"
               "mov %1, %%ds;"
               "mov %%ds:0(%2), %0;"
               "test $1, %0;"
               "jz 1f;"
               "mov %%ds:0(%3), %0;"
               "movl $0, %%ds:0(%3);"
               "1:"
               "pop %%ds;"
               : "=&r"(value)
               : "r"(SEL_ALL_DATA),
                 "r"(page_dir + page_dir_index),
                 "r"(page_table + page_table_index)
               : "cc");
  invalidate(virtual);
  return value & ~0x3ff;
}

uintptr_t mmap_remap_page(void* virtual, unsigned attributes) {
  //printf("Remapping virtual address = %x\n", (unsigned)virtual);
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  uintptr_t physical = 0;
  uint32_t* page_dir = mmap.page_directory;
  uint32_t* page_tables = mmap.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;

  // ensure that the page table and page exists; reset the page entry.
  unsigned dir_value, table_value;
  asm volatile("push %%ds;"
               "mov %2, %%ds;"
               "mov %%ds:0(%3), %0;"
               "test $1, %0;"
               "jz 1f;"
               "mov %%ds:0(%5), %1;"
               "test $1, %1;"
               "jz 1f;"
               "andl $0xfffff000, %1;"
               "or %4, %1;"
               "mov %1, %%ds:0(%5);"
               "1:"
               "pop %%ds;"
               : "=&r"(dir_value), "=&r"(table_value)
               : "r"(SEL_ALL_DATA),
                 "r"(page_dir + page_dir_index),
                 "r"(attributes),
                 "r"(page_table + page_table_index)
               : "cc");

  invalidate(virtual);
  return physical;
}

void mmap_mirror_page(void* destination, void* source) {
  uintptr_t dst_address = (uintptr_t)destination;
  unsigned dst_page_dir_index = dst_address >> 22;
  unsigned dst_page_table_index = (dst_address >> 12) & 0x3ff;

  uintptr_t src_address = (uintptr_t)source;
  unsigned src_page_dir_index = src_address >> 22;
  unsigned src_page_table_index = (src_address >> 12) & 0x3ff;

  uint32_t* page_dir = mmap.page_directory;
  uint32_t* page_tables = mmap.page_tables;
  uint32_t *dst_page_table = page_tables + dst_page_dir_index * 0x400;
  uint32_t *src_page_table = page_tables + src_page_dir_index * 0x400;
  
  // ensure that the page tables exists; copy the page entry.
  unsigned value;
  asm volatile("push %%ds;"
               "mov %1, %%ds;"
               "mov %%ds:0(%2), %0;"
               "test $1, %0;"
               "jz 1f;"
               "mov %%ds:0(%3), %0;"
               "test $1, %0;"
               "jz 1f;"
               "mov %%ds:0(%4), %0;"
               "mov %0, %%ds:0(%5);"
               "mov $1, %0;"
               "1:"
               "pop %%ds;"
               : "=&r"(value)
               : "r"(SEL_ALL_DATA),
                 "r"(page_dir + src_page_dir_index),
                 "r"(page_dir + dst_page_dir_index),
                 "r"(src_page_table + src_page_table_index),
                 "r"(dst_page_table + dst_page_table_index)
               : "cc");
  if ((value & 1) == 0) {
    // TODO: we can skip this test; then it'd create a page fault instead.
    printf("Page mirror error: %x ti %x!\n",
           (unsigned)source, (unsigned)destination);
    abort();
  }

  invalidate(destination);
}

void* mmap_map(void* virtual, uintptr_t physical,
               size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  if ((((uintptr_t)virtual) % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    printf("Virtual and physical addresses must be page aligned!\n");
    abort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_map_page(virtual + i, physical + i, attributes);
  }
  return virtual;
}

void mmap_unmap(void* virtual, size_t size) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  if (((uintptr_t)virtual) % PAGE_SIZE) {
    printf("Virtual address must be page aligned!\n");
    abort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_unmap_page(virtual + i);
  }
}

void mmap_remap(void* virtual, size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  if (((uintptr_t)virtual) % PAGE_SIZE) {
    printf("Virtual address must be page aligned!\n");
    abort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_remap_page(virtual + i, attributes);
  }
}

void mmap_mirror(void* destination, void* source, size_t size) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  if (((uintptr_t)destination) % PAGE_SIZE) {
    printf("Virtual address must be page aligned!\n");
    abort();
  }
  if (((uintptr_t)source) % PAGE_SIZE) {
    printf("Virtual address must be page aligned!\n");
    abort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_mirror_page(destination + i, source + i);
  }
}

void* mmap_map_table(void* virtual, uintptr_t page_table, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  if (address & 0x3fffff) {
    printf("Page table must be mapped at table boundary.\n");
    abort();
  }
  unsigned page_dir_index = address >> 22;
  uint32_t* page_dir = mmap.page_directory;

  // just set the page directory entry.
  asm volatile("push %%ds;"
               "mov %0, %%ds;"
               "mov %1, %%ds:0(%2);"
               "pop %%ds;"
               :
               : "r"(SEL_ALL_DATA),
                 "r"(page_table | attributes),
                 "r"(page_dir + page_dir_index)
               );

  invalidate_all(); // TODO: or just all pages in the page table.
  return virtual;
}

void* mmap_temp_map(uintptr_t page, unsigned attributes) {
  // TODO: critical section until unmap called.
  return mmap_map_page(mmap_tmp_page, page, attributes);
}

void mmap_temp_unmap(void* address) {
  (void)(address);//we just have one tmp_page for now..
  mmap_unmap_page(mmap_tmp_page);
}
