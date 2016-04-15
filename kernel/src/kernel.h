#ifndef __HELLBENDER_KERNEL_H__
#define __HELLBENDER_KERNEL_H__

#include <stdint.h>

#include "config.h"
#include "multiboot.h"
#include "elf32.h"

#define MAX_MEMORY_MAPS 16
#define MAX_MODULES     16
#define MAX_HEADERS     16

struct MultibootData {
  memory_map_t mem_maps[MAX_MEMORY_MAPS];
  unsigned n_mem_maps;

  module_t modules[MAX_MODULES];
  unsigned n_modules;
    
  Elf32_Shdr headers[MAX_HEADERS];
  unsigned n_headers;

  uintptr_t memory_bottom;
  uintptr_t memory_top;
  uintptr_t allocated_top; // anything above this is free.
};

INLINE uintptr_t kernel_v2p(void* virtual) { return ((uintptr_t)virtual) & KERNEL_VMASK; }
INLINE void* kernel_p2v(uintptr_t physical) { return (void*)(physical + KERNEL_OFFSET); }

__attribute__((__noreturn__))
void kernel_panic();

void kernel_main(struct MultibootData* data);

#endif
