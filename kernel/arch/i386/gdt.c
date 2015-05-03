#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/kernel.h>
#include <kernel/gdt.h>

uint8_t gdt_table[SEL_COUNT*8];

typedef struct __attribute__((packed)) gdt_address {
  uint16_t size;
  uint32_t address;
} gdt_address_t;

void gdt_early_set_entry(unsigned selector, gdt_entry_t source) {
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

void gdt_early_enable_segments() {
#ifdef DEBUG
  printf("gdt_early_enable_segments\n");
#endif

  volatile gdt_address_t gdt // needs volatile because some wierd optimization.
    = (gdt_address_t) { .address = (uint32_t)gdt_table,
                        .size = sizeof(gdt_table) };
  uintptr_t gdt_address = (uintptr_t)&gdt;
  asm ("cli;"
       "lgdt (%%eax);"
       "ljmp $0x08, $1f;"
       "1:"
       "mov %2, %%ax;"
       "mov %%ax, %%ds;"
       "mov %%ax, %%es;"
       "mov %%ax, %%fs;"
       "mov %%ax, %%gs;"
       "mov %%ax, %%ss;"
       : /* no output registers */
       : "a"(gdt_address), "n"(SEL_KERNEL_CODE), "n"(SEL_KERNEL_DATA)
       : 
       );
}
