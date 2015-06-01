#ifndef _KERNEL_MODULE_H
#define _KERNEL_MODULE_H

#include <stdint.h>

typedef struct module_binary {
  uintptr_t bottom;
  uintptr_t top;
} module_binary_t;

typedef struct kernel_module {
  uint32_t bottom;
  uint32_t top;
  uint32_t ro_bottom;   // read-only area; inside bottom..top
  uint32_t ro_top;
  uint32_t text_bottom; // executable area; inside ro_bottom..ro_top.
  uint32_t text_top;
  uint32_t start;       // entry point; inside text_bottom..text_top.
  void* module_info;    // pointer to core_service structure (or zero).
} kernel_module_t;

#define CORE_INIT_STAGE_BEGIN 0
#define CORE_INIT_STAGE_READY_FOR_INTERRUPTS 1
#define CORE_INIT_STAGE_INTERRUPTS_ACTIVATED 2

// core service module is loaded by GRUB.
// core provides some basic services to get things running.
typedef struct core_service {
  uint32_t this_size;
  uint32_t module_start;
  uint32_t init_stage;
  uint32_t timer_isr;
  uint32_t ps2_chan1_isr;
  uint32_t ps2_chan2_isr;
  uint32_t vga_buffer;
  uint32_t initrd_buffer;
  uint32_t initrd_size;
  uint32_t process_bootstrap;
} __attribute__((packed)) core_service_t;

int module_parse_elf(module_binary_t *binary, kernel_module_t *module);

#endif
