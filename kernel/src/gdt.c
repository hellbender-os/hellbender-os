#include "kernel.h"

struct gdt {
  uint16_t limt_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access; // present,privl1,privl0,1,exec,dir(!exec)/conf(exec),rw,0
  uint8_t granularity;
  uint8_t base_high;
} __attribute__ ((__packed__));

struct gdt gdt[N_SELECTORS] __attribute__((aligned(0x20))) = {
  { 0, 0, 0, 0, 0, 0 }, // null descriptor
  { 0, 0, 0, 0b10011010, 0b00100000, 0 }, // kernel code descriptor
  { 0, 0, 0, 0b10010010, 0b00000000, 0 }, // kernel data descriptor
  { 0, 0, 0, 0b11111010, 0b00100000, 0 }, // user code descriptor
  { 0, 0, 0, 0b11110010, 0b00000000, 0 }, // user data descriptor
};

struct __attribute__ ((__packed__)) {
  uint16_t limit;
  uint64_t base;
} gdt_ptr __attribute__((aligned(0x20))) = {
  (uint16_t)sizeof(gdt), 
  (uint64_t)gdt
};

