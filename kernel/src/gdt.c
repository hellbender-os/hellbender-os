#include "gdt.h"
#include "tss.h"
#include "kernel.h"

gdt_t gdt = {
  {
    { 0, 0, 0, 0, 0, 0 },                   // null descriptor
    { 0, 0, 0, 0b10011010, 0b00100000, 0 }, // kernel code descriptor
    { 0, 0, 0, 0b10010010, 0b00000000, 0 }, // kernel data descriptor
    { 0, 0, 0, 0b11111010, 0b00100000, 0 }, // user code descriptor
    { 0, 0, 0, 0b11110010, 0b00000000, 0 }, // user data descriptor
  },
  { sizeof(tss_t), 0, 0, 0x89, 0, 0, 0, 0 } // TSS descriptor
};


struct __attribute__ ((__packed__)) {
  uint16_t limit;
  uint64_t base;
} gdt_ptr __attribute__ ((aligned(0x20))) = {
  (uint16_t)sizeof(gdt), 
  (uint64_t)&gdt
};

void gdt_init() {
  // append TSS descriptor.
  uintptr_t address = (uintptr_t)&cpu.tss;
  gdt.tssd.base_address_0 = (address & 0xffff);
  gdt.tssd.base_address_1 = ((address >> 16) & 0xff);
  gdt.tssd.base_address_2 = ((address >> 24) & 0xff);
  gdt.tssd.base_address_3 = ((address >> 32) & 0xffffffff);
}

