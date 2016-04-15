#include "kernel.h"
#include "vga.h"
#include "page.h"
#include "lomem.h"
#include "himem.h"

#include <string.h>

struct MultibootData boot_64_data;

void boot_64(uint32_t magic, uint32_t multiboot) {
  multiboot_info_t *info = (multiboot_info_t *)((uintptr_t)multiboot);
  // this is lower half address but that is ok, since the first 1GB
  // is initially mapped at 0 and KERNEL_OFFSET.

  // checking multiboot magic variable.
  VGA_AT(0,0) = VGA_ENTRY('H', WHITE_ON_BLACK);
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    VGA_AT(0,1) = VGA_ENTRY('!', WHITE_ON_BLACK);
    kernel_panic();
  }

  // copying multiboot data.
  VGA_AT(0,1) = VGA_ENTRY('E', WHITE_ON_BLACK);
  boot_64_data.memory_bottom = -1;
  uintptr_t mm_end = (uintptr_t)info->mmap_addr + info->mmap_length;
  for (uintptr_t mm = (uintptr_t)info->mmap_addr; mm < mm_end; mm += sizeof(uint32_t)) {
    memory_map_t *map = (memory_map_t*)mm;
    if (map->type == 1) {
      if (boot_64_data.n_mem_maps > MAX_MEMORY_MAPS) {
	VGA_AT(0,2) = VGA_ENTRY('?', WHITE_ON_BLACK);
        kernel_panic();
      }
      memcpy(&boot_64_data.mem_maps[boot_64_data.n_mem_maps++],
             map, sizeof(memory_map_t));
      uintptr_t base = ((uintptr_t)map->base_addr_low) 
        + (((uintptr_t)map->base_addr_high)<<32);
      uintptr_t size = ((uint64_t)map->length_low) 
        + (((uint64_t)map->length_high)<<32);
      uintptr_t top = base + size;
      if (base < boot_64_data.memory_bottom) boot_64_data.memory_bottom = base;
      if (top > boot_64_data.memory_top) boot_64_data.memory_top = top;
    }
    mm += map->size;
  }
  boot_64_data.memory_bottom &= TABLE_ADDRESS_MASK;
  boot_64_data.memory_top -= boot_64_data.memory_top % TABLE_SIZE;

  if (info->mods_count > MAX_MODULES) {
    VGA_AT(0,2) = VGA_ENTRY('?', WHITE_ON_BLACK);
    kernel_panic();
  }
  boot_64_data.n_modules = info->mods_count;
  memcpy(boot_64_data.modules,
         (module_t*)((uintptr_t)info->mods_addr), 
         boot_64_data.n_modules * sizeof(module_t));
  for (unsigned k = 0; k < boot_64_data.n_modules; ++k) {
    module_t *m = boot_64_data.modules + k;
    uintptr_t mod_top = page_round_up(m->mod_end);
    if (mod_top > boot_64_data.allocated_top) boot_64_data.allocated_top = mod_top;
  }

  if (info->u.elf_sec.num > MAX_HEADERS) {
    VGA_AT(0,2) = VGA_ENTRY('.', WHITE_ON_BLACK);
    kernel_panic();
  }
  boot_64_data.n_headers = info->u.elf_sec.num;
  memcpy(boot_64_data.headers,
         (Elf32_Shdr*)((uintptr_t)info->u.elf_sec.addr),
         boot_64_data.n_headers * sizeof(Elf32_Shdr));    
  for (unsigned k = 0; k < boot_64_data.n_headers; ++k) {
    Elf32_Shdr *h = boot_64_data.headers + k;
    uintptr_t mod_top = page_round_up(h->sh_addr + h->sh_size);
    if (mod_top > boot_64_data.allocated_top) boot_64_data.allocated_top = mod_top;
  }

  // we no longer need the original memory map at zero address.
  page_set_pdpt(0, 0, 0);
  page_invalidate_all();
  
  // initialize page allocators.
  VGA_AT(0,2) = VGA_ENTRY('L', WHITE_ON_BLACK);
  lomem_init(&boot_64_data);
  BREAK;
  himem_init(&boot_64_data);

  // start setting up kernel structures.
  VGA_AT(0,3) = VGA_ENTRY('L', WHITE_ON_BLACK);
  kernel_main(&boot_64_data);
}
