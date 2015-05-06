#ifndef _KERNEL_MODULE_H
#define _KERNEL_MODULE_H

#include <stdint.h>

typedef struct kernel_module {
  uint32_t magic; // 0x1337c0de;
  uint32_t bottom;
  uint32_t top;
  uint32_t text_bottom;
  uint32_t text_top;
  uint32_t start;
  uint32_t checksum; // cast structure as uint16_t array, excluding the checksum field. checksum == the sum of the array.
} __attribute__((packed)) kernel_module_t;

int module_check_header(kernel_module_t *module);

#endif
