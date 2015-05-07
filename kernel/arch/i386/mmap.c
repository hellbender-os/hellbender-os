#include <stddef.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/mmap.h>
#include <kernel/mem.h>

typedef struct mmap {
  // a range of free pages that we are consuming next.
  uintptr_t free_pages;
  size_t free_size;
  
  uint32_t *page_directory; // virtual address to the recursive page directory.
  uint32_t *page_tables; // virtual address to the recursive page tables.
} mmap_t;
mmap_t mmap;

uint8_t mmap_page_directory[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mmap_page_table_0[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mmap_page_table_1[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

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

static inline unsigned set_ds_all() {
  unsigned old_ds;
  asm volatile("mov %%ds, %0;"
               "mov %2, %%ds;"
               "mov %2, %%es;"
               : "=&r"(old_ds), "=m"(__force_order)
               : "r"(SEL_ALL_DATA)
               : "memory"
               );
  return old_ds;
}

static inline void reset_ds(unsigned old_ds) {
  asm volatile("mov %1, %%ds;"
               "mov %1, %%es;"
               : "=m"(__force_order)
               : "r"(old_ds)
               : "memory"
               );
}

void mmap_early_initialize() {
  memset(&mmap, 0, sizeof(mmap));
  memset(mmap_page_directory, 0, PAGE_SIZE);
  memset(mmap_page_table_0, 0, PAGE_SIZE);
  memset(mmap_page_table_1, 0, PAGE_SIZE);

  uint32_t *page_dir = (uint32_t*)mmap_page_directory;

  // map the first two page tables for DS and CS segments.
  page_dir[0] = ((uint32_t)(mmap_page_table_0)) | MMAP_ATTRIB_KERNEL;
  page_dir[CS_BASE >> 22] =
    ((uint32_t)(mmap_page_table_1)) | MMAP_ATTRIB_KERNEL;
  
  // recursive page directory is mapped to the last 4MB.
  // note that access requires SEL_ALL_DATA selector.
  page_dir[1023] = ((uint32_t)page_dir) | MMAP_ATTRIB_KERNEL;
  mmap.page_tables = (uint32_t*)((uintptr_t)1023 * (uintptr_t)0x400000);
  mmap.page_directory = (uint32_t*)((uintptr_t)(mmap.page_tables)
                                    + (uintptr_t)1023 * (uintptr_t)0x1000);
}

void mmap_early_finalize() {
  // hide page tables from the data segment.
  mmap_unmap_page(&mmap_page_directory);
  mmap_unmap_page(&mmap_page_table_0);
  mmap_unmap_page(&mmap_page_table_1);
}

void* mmap_early_map_page(void* virtual, uintptr_t page, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  // page directory and page tables are mapped as part of the kernel.
  if ((address % PAGE_SIZE) || (page % PAGE_SIZE)) {
    kprintf("Address and page must be page aligned!\n");
    kabort();
  }
  uint32_t *page_dir = (uint32_t*)mmap_page_directory;
  uint32_t page_dir_index = address >> 22;
  uint32_t page_table_index = (address >> 12) & 0x3ff;
  //kprintf("PDI = %u; PTI = %u; attr = %u\n",
  //	 (unsigned)page_dir_index, (unsigned)page_table_index,
  //	 (unsigned)attributes);
  if ((page_dir[page_dir_index] & 1) == 0) {
    kprintf("mmap_early_map_page supports only the first 4MB region!\n");
    kabort();
  }
  uint32_t *page_table = (uint32_t*)(page_dir[page_dir_index] & 0xfffff000);
  page_table[page_table_index] = (page & 0xfffff000) | attributes;
  invalidate(virtual);
  return virtual;
}

void* mmap_early_map(void* virtual, uintptr_t physical,
                     size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    kprintf("Virtual and physical addresses must be page aligned!\n");
    kabort();
  }
  for (uint32_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_early_map_page(virtual + i, physical + i, attributes);
  }
  return virtual;
}

void mmap_early_enable_paging() {
#ifdef DEBUG
  kprintf("mmap_early_enable_paging\n");
#endif
  
  asm ("mov %%eax, %%cr3;"
       "mov %%cr0, %%eax;"
       "or $0x80000000, %%eax;"
       "mov %%eax, %%cr0;"
       : /* no output registers */
       : "a"(&mmap_page_directory)
       ); 
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
  unsigned old_ds = set_ds_all();
  if ((page_dir[page_dir_index] & 1) == 0) {
    uintptr_t new_table = mem_alloc_page();
    page_dir[page_dir_index] = new_table | attributes;
    invalidate(page_table);
    memset(page_table, 0, PAGE_SIZE);
  }

  // set the page table entry.
  page_table[page_table_index] = (physical & 0xfffff000) | attributes;
  invalidate(virtual);
  reset_ds(old_ds);
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

  unsigned old_ds = set_ds_all();
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
  reset_ds(old_ds);
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

  unsigned old_ds = set_ds_all();
  if (page_dir[page_dir_index] & 1) {
    // get and clear the page table entry.
    uint32_t value = page_table[page_table_index];
    if (value & 1) {
      physical = value & 0xfffff000;
      page_table[page_table_index] = physical | attributes;
      asm volatile("invlpg (%0)" ::"r"(address) : "memory");
    }
  }
  reset_ds(old_ds);
  return physical;
}

void* mmap_map(void* virtual, uintptr_t physical,
               size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    kprintf("Virtual and physical addresses must be page aligned!\n");
    kabort();
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
    kprintf("Virtual address must be page aligned!\n");
    kabort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_unmap_page(virtual + i);
  }
}

void mmap_remap(void* virtual, size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE)) {
    kprintf("Virtual address must be page aligned!\n");
    kabort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mmap_remap_page(virtual + i, attributes);
  }
}

void* mmap_map_table(void* virtual, uintptr_t page_table, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  if (address & 0x3fffff) {
    kprintf("Page table must be mapped at table boundary.\n");
    kabort();
  }
  unsigned page_dir_index = address >> 22;
  uint32_t* page_dir = mmap.page_directory;

  unsigned old_ds = set_ds_all();
  page_dir[page_dir_index] = page_table | attributes;
  reset_ds(old_ds);
  invalidate_all(); // TODO: or just all pages in the page table.
  return virtual;
}
