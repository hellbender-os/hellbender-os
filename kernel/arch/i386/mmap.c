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

static inline unsigned set_segments() {
  unsigned old_ds;
  asm volatile("mov %%ds, %0;"
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
  printf("mapping %x-%x into %x-%x\n",
         (unsigned)physical, (unsigned)(physical+size),
         (unsigned)virtual, (unsigned)(virtual+size));
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
}

void* mmap_map_page(void* virtual, uintptr_t physical, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  //kprintf("Mapping virtual address = %x\n", (unsigned)virtual);
  //kprintf("PDI = %u; PTI = %u; attr = %u\n",
  //        (unsigned)page_dir_index, (unsigned)page_table_index,
  //        (unsigned)attributes);

  uint32_t* page_dir = mmap.page_directory;
  uint32_t* page_tables = mmap.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;
  
  // ensure that the is a page table.
  unsigned old_ds = set_segments();
  if ((page_dir[page_dir_index] & 1) == 0) {
    // TODO: we can skip this test; then it'd create a page fault instead.
    //uintptr_t new_table = mem_alloc_page();
    //page_dir[page_dir_index] = new_table | attributes;
    //invalidate(page_table);
    //memset(page_table, 0, PAGE_SIZE);
    printf("Trying to map %x to unmapped page table!\n", (unsigned)virtual);
    abort();
  }

  // set the page table entry.
  page_table[page_table_index] = (physical & 0xfffff000) | attributes;
  invalidate(virtual);
  reset_segments(old_ds);
  return virtual;
}

uintptr_t mmap_unmap_page(void* virtual) {
  //kprintf("Unmapping virtual address = %x\n", (unsigned)virtual);
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  uintptr_t physical = 0;
  uint32_t* page_dir = mmap.page_directory;
  uint32_t* page_tables = mmap.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;

  unsigned old_ds = set_segments();
  if (page_dir[page_dir_index] & 1) {
    // get and clear the page table entry.
    uint32_t value = page_table[page_table_index];
    if (value & 1) {
      physical = value & 0xfffff000;
      page_table[page_table_index] = 0;
      //kprintf("unmapping PDI=%x; PTI=%x, was %x\n",
      //       (unsigned)page_dir_index, (unsigned)page_table_index,
      //       (unsigned)physical);
      asm volatile("invlpg (%0)" ::"r"(address) : "memory");
    }
  }
  reset_segments(old_ds);
  return physical;
}

uintptr_t mmap_remap_page(void* virtual, unsigned attributes) {
  //kprintf("Remapping virtual address = %x\n", (unsigned)virtual);
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  uintptr_t physical = 0;
  uint32_t* page_dir = mmap.page_directory;
  uint32_t* page_tables = mmap.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;

  unsigned old_ds = set_segments();
  if (page_dir[page_dir_index] & 1) {
    // get and clear the page table entry.
    uint32_t value = page_table[page_table_index];
    if (value & 1) {
      physical = value & 0xfffff000;
      page_table[page_table_index] = physical | attributes;
      asm volatile("invlpg (%0)" ::"r"(address) : "memory");
    }
  }
  reset_segments(old_ds);
  return physical;
}

void* mmap_map(void* virtual, uintptr_t physical,
               size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
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
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE)) {
    printf("Virtual address must be page aligned!\n");
    abort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_unmap_page(virtual + i);
  }
}

void mmap_remap(void* virtual, size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE)) {
    printf("Virtual address must be page aligned!\n");
    abort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_remap_page(virtual + i, attributes);
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

  unsigned old_ds = set_segments();
  page_dir[page_dir_index] = page_table | attributes;
  reset_segments(old_ds);
  invalidate_all(); // TODO: or just all pages in the page table.
  return virtual;
}
