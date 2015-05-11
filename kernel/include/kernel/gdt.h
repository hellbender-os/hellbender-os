#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stddef.h>

typedef struct gdt_entry {
  uint32_t base;
  uint32_t limit;
  uint8_t type;
} gdt_entry_t;

extern uint8_t gdt_table[];

void gdt_stage_2_init();

#endif
