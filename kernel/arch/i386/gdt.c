#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/gdt.h>
#include <kernel/tss.h>

uint8_t gdt_table[SEL_COUNT*8];

static void gdt_enable_segments();
static void gdt_set_entry(unsigned selector, gdt_entry_t source);

typedef struct __attribute__((packed)) gdt_address {
  uint16_t size;
  uint32_t address;
} gdt_address_t;

void gdt_stage_2_init() {
  memset(gdt_table, 0, sizeof(gdt_table));

  // build global descriptor table.
  gdt_entry_t gdt[SEL_COUNT] = {{0}};
  gdt[0] = (gdt_entry_t) { // 0x00 == NULL
    .base  = 0,
    .limit = 0,
    .type  = 0
  };
  gdt[SEL_KERNEL_CODE/8] = (gdt_entry_t) {
    .base  = CS_BASE,
    .limit = 0xffbfffff-CS_BASE, // last 4MB reserved for recursive page dir.
    .type  = 0x9A
  };
  gdt[SEL_KERNEL_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = CS_BASE-1,
    .type  = 0x92
  };
  gdt[SEL_USER_CODE/8] = (gdt_entry_t) {
    .base  = CS_BASE,
    .limit = 0xffbfffff-CS_BASE,
    .type  = 0xFA
  };
  gdt[SEL_USER_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = CS_BASE-1,
    .type  = 0xF2
  };
  gdt[SEL_ALL_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = 0xffffffff,
    .type  = 0x92
  };

  // also setup the TSS.
  memset(&kernel_tss, 0, sizeof(kernel_tss));
  // TSS stack is always mapped at a fixed address.
  // it is used just to store the context data.
  kernel_tss.ss0  = SEL_KERNEL_DATA;
  kernel_tss.esp0 = THREAD_TSS_STACK_TOP;
  gdt[SEL_TSS/8] = (gdt_entry_t) {
    .base = (uintptr_t)&kernel_tss,
    .limit = sizeof(kernel_tss),
    .type = 0x89
  };

  for (int i = 0; i < SEL_COUNT; ++i) {
    gdt_set_entry(i*8, gdt[i]);
  }

  // switch to segmented mode.
  gdt_enable_segments();
  tss_update();
}

static void gdt_set_entry(unsigned selector, gdt_entry_t source) {
  uint8_t* target  = gdt_table + selector;
  // Check the limit to make sure that it can be encoded
  if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) {
    printf("Illegal segment limit!");
    abort();
  }
  if (source.limit > 65536) {
    // Adjust granularity if required
    source.limit = source.limit >> 12;
    target[6] = 0xC0;
  } else {
    target[6] = 0x40;
  }
 
  // Encode the limit
  target[0] = source.limit & 0xFF;
  target[1] = (source.limit >> 8) & 0xFF;
  target[6] |= (source.limit >> 16) & 0xF;
  
  // Encode the base 
  target[2] = source.base & 0xFF;
  target[3] = (source.base >> 8) & 0xFF;
  target[4] = (source.base >> 16) & 0xFF;
  target[7] = (source.base >> 24) & 0xFF;
  
  // And... Type
  target[5] = source.type;
}

static void gdt_enable_segments() {
  volatile gdt_address_t gdt // needs volatile because some wierd optimization.
    = (gdt_address_t) { .address = (uint32_t)gdt_table,
                        .size = sizeof(gdt_table)-1 };
  asm ("lgdt (%%eax);"
       "ljmp %1, $1f;"
       "1:"
       "mov %2, %%ax;"
       "mov %%ax, %%ds;"
       "mov %%ax, %%es;"
       "mov %%ax, %%fs;"
       "mov %%ax, %%gs;"
       "mov %%ax, %%ss;"
       : /* no output registers */
       : "a"(&gdt), "n"(SEL_KERNEL_CODE), "n"(SEL_KERNEL_DATA)
       :
       );
}
