#ifndef __HELLBENDER_KERNEL_GDT_H__
#define __HELLBENDER_KERNEL_GDT_H__

#include "config.h"

#include <stdint.h>

struct gdt_entry {
  uint16_t limt_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access; // present,privl1,privl0,1,exec,dir(!exec)/conf(exec),rw,0
  uint8_t granularity;
  uint8_t base_high;
} __attribute__ ((__packed__));

struct tss_entry {
  uint16_t segment_limit;
  uint16_t base_address_0;
  uint8_t base_address_1;
  uint8_t access; // present,privl2,0,type4
  uint8_t granularity;
  uint8_t base_address_2;
  uint32_t base_address_3;
  uint32_t reserved;
} __attribute__ ((packed));

typedef struct __attribute__ ((packed)) {
  struct gdt_entry gdtd[N_SELECTORS];
  struct tss_entry tssd;
} __attribute__((aligned(0x20))) gdt_t;

void gdt_init();

#endif
