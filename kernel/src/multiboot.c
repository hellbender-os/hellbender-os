#include "multiboot.h"
#include "config.h"
#include "page.h"

#include <string.h>

#define FLAGS (MULTIBOOT_FLAGS_ALIGN | MULTIBOOT_FLAGS_MEMINFO)

multiboot_header_t multiboot_header __attribute__ ((section (".multiboot"))) = {
  MULTIBOOT_HEADER_MAGIC,
  FLAGS,
  -(MULTIBOOT_HEADER_MAGIC + FLAGS),
  0, 0, 0, 0, 0
};

struct multiboot_data multiboot_data;

int multiboot_copy(multiboot_info_t *info) {
  // copy memory info, find memory limits.
  multiboot_data.memory_bottom = -1;
  uintptr_t mm_end = (uintptr_t)info->mmap_addr + info->mmap_length;
  for (uintptr_t mm = (uintptr_t)info->mmap_addr; mm < mm_end; mm += sizeof(uint32_t)) {
    memory_map_t *map = (memory_map_t*)mm;
    if (map->type == 1) {
      if (multiboot_data.n_mem_maps > MAX_MEMORY_MAPS) {
        return MULTIBOOT_TOO_MANY_MEMORY_MAPS;
      }
      memcpy(&multiboot_data.mem_maps[multiboot_data.n_mem_maps++],
             map, sizeof(memory_map_t));
      uintptr_t base = ((uintptr_t)map->base_addr_low) 
        + (((uintptr_t)map->base_addr_high)<<32);
      uintptr_t size = ((uint64_t)map->length_low) 
        + (((uint64_t)map->length_high)<<32);
      uintptr_t top = base + size;
      if (base < multiboot_data.memory_bottom) multiboot_data.memory_bottom = base;
      if (top > multiboot_data.memory_top) multiboot_data.memory_top = top;
    }
    mm += map->size;
  }
  multiboot_data.memory_bottom &= TABLE_VIRTUAL_MASK;
  multiboot_data.memory_top -= multiboot_data.memory_top % TABLE_SIZE;

  // copy module info, find memory limits, find coresrv, rootfs.
  if (info->mods_count > MAX_MODULES) {
    return -1;
  }
  multiboot_data.n_modules = info->mods_count;
  memcpy(multiboot_data.modules,
         (module_t*)((uintptr_t)info->mods_addr), 
         multiboot_data.n_modules * sizeof(module_t));
  multiboot_data.coresrv_module = -1;
  multiboot_data.initrd_module = -1;
  for (unsigned k = 0; k < multiboot_data.n_modules; ++k) {
    module_t *m = multiboot_data.modules + k;
    uintptr_t mod_top = page_round_up(m->mod_end);
    if (mod_top > multiboot_data.allocated_top) multiboot_data.allocated_top = mod_top;
    if (strcmp((char*)(uint64_t)m->string, "--coresrv") == 0) {
      multiboot_data.coresrv_module = k;
    }
    if (strcmp((char*)(uint64_t)m->string, "--initrd") == 0) {
      multiboot_data.initrd_module = k;
    }
  }

  // copy kernel ELF info, find memory limits.
  if (info->u.elf_sec.num > MAX_HEADERS) {
    return MULTIBOOT_TOO_MANY_ELF_HEADERS;
  }
  multiboot_data.n_headers = info->u.elf_sec.num;
  memcpy(multiboot_data.headers,
         (Elf32_Shdr*)((uintptr_t)info->u.elf_sec.addr),
         multiboot_data.n_headers * sizeof(Elf32_Shdr));    
  for (unsigned k = 0; k < multiboot_data.n_headers; ++k) {
    Elf32_Shdr *h = multiboot_data.headers + k;
    uintptr_t mod_top = page_round_up(h->sh_addr + h->sh_size);
    if (mod_top > multiboot_data.allocated_top) multiboot_data.allocated_top = mod_top;
  }

  return 0;
}

