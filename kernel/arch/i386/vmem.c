#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/vmem.h>

uint8_t vmem_free_tables[64]; // bit map of 512 tables, free if bit = 1.

void vmem_stage_3_init() {
  memset(vmem_free_tables, 0xff, sizeof(vmem_free_tables));

  // first table for kernel.
  // second table for current thread.
  // last for recursive page directory.
  // TODO: it would be better to do accocation explicitely from kernel_init.
  vmem_free_tables[0] &= ~1;
  vmem_free_tables[511/8] &= ~128; 
}

void* vmem_alloc_table(unsigned table) {
  if (table) {
    vmem_free_tables[table/8] &= ~(1<<(table%8));
    return (void*)((uintptr_t)table * (uintptr_t)0x400000);
  } else {
    printf("Out of virtual memory tables!\n");
    abort();
  }
}

void* vmem_alloc_bottom_table() {
  unsigned table = 0;
  for (int i = 0; i < 64 && !table; ++i) {
    uint8_t bits;
    if ((bits = vmem_free_tables[i])) {
      if (bits &   1) { table = i*8+0; break; }
      if (bits &   2) { table = i*8+1; break; }
      if (bits &   4) { table = i*8+2; break; }
      if (bits &   8) { table = i*8+3; break; }
      if (bits &  16) { table = i*8+4; break; }
      if (bits &  32) { table = i*8+5; break; }
      if (bits &  64) { table = i*8+6; break; }
      if (bits & 128) { table = i*8+7; break; }
    }
  }
  return vmem_alloc_table(table);
}

void* vmem_alloc_top_table() {
  unsigned table = 0;
  for (int i = 64; i >= 0 && !table; --i) {
    uint8_t bits;
    if ((bits = vmem_free_tables[i])) {
      if (bits & 128) { table = i*8+7; break; }
      if (bits &  64) { table = i*8+6; break; }
      if (bits &  32) { table = i*8+5; break; }
      if (bits &  16) { table = i*8+4; break; }
      if (bits &   8) { table = i*8+3; break; }
      if (bits &   4) { table = i*8+2; break; }
      if (bits &   2) { table = i*8+1; break; }
      if (bits &   1) { table = i*8+0; break; }
    }
  }
  return vmem_alloc_table(table);
}

void* vmem_alloc_existing_table(void* existing) {
  unsigned table = (uintptr_t)existing / (uintptr_t)TABLE_SIZE;
  if (!(vmem_free_tables[table/8] & (1<<(table%8)))) {
    printf("Conflicting page table allocations!\n");
    abort();
  }
  return vmem_alloc_table(table);
}
