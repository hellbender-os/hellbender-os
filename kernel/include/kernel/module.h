#ifndef _KERNEL_MODULE_H
#define _KERNEL_MODULE_H

#include <stdint.h>

typedef struct module_binary {
  uintptr_t bottom;
  uintptr_t top;
} module_binary_t;

typedef struct kernel_module {
  uint32_t magic; // 0x1337c0de;
  uint32_t bottom;
  uint32_t top;
  uint32_t text_bottom;
  uint32_t text_top;
  uint32_t start;
  uint32_t module_info;
  uint32_t checksum; // cast structure as uint16_t array, excluding the checksum field. checksum == the sum of the array.
} __attribute__((packed)) kernel_module_t;


// core module is mapped at a fixed address at the top of the address space.
#define CORE_OFFSET 0x7F800000

typedef struct core_service {
  uint32_t this_size;
  uint32_t keyboard_isr;
  uint32_t timer_isr;
} __attribute__((packed)) core_service_t;

#define CORE_MODULE ((kernel_module_t*)CORE_OFFSET)
#define CORE_SERVICE ((core_service_t*)(CORE_OFFSET + sizeof(kernel_module_t)))


int module_check_header(kernel_module_t *module);

#endif
