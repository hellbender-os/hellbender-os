#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stddef.h>

typedef struct gdt_entry {
  uint32_t base;
  uint32_t limit;
  uint8_t type;
} gdt_entry_t;

void gdt_early_initialize();
void gdt_early_set_entry(unsigned selector, gdt_entry_t source);
void gdt_early_enable_segments();

#endif
