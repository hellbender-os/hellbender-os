#ifndef __HELLBENDER_KERNEL_TSS_H__
#define __HELLBENDER_KERNEL_TSS_H__

#include "config.h"

// A struct describing a Task State Segment.
typedef struct __attribute__ ((packed)) {
  uint32_t reserved_0;
  uint64_t rsp_0;
  uint64_t rsp_1;
  uint64_t rsp_2;
  uint64_t reserved_1;
  uint64_t ist_1;
  uint64_t ist_2;
  uint64_t ist_3;
  uint64_t ist_4;
  uint64_t ist_5;
  uint64_t ist_6;
  uint64_t ist_7;
  uint64_t reserved_2;
  uint16_t reserved_3;
  uint16_t iomap_base;
} tss_t __attribute__ ((aligned(0x20)));

INLINE void tss_update() {
  asm volatile ("ltr %%ax"
                : // no output registers
                : "a"(TSS_SELECTOR|3)
                :
                );
}

#endif
