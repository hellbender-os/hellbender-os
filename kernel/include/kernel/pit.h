#ifndef _KERNEL_PIT_H
#define _KERNEL_PIT_H

#define PIT_BASE_FREQUENCY 1193182
#define PIT_TICKS_PER_SECOND 100
#define PIT_PULSES_PER_TICK (PIT_BASE_FREQUENCY/PIT_TICKS_PER_SECOND)

#define PIT_CHAN0_DATA 0x40
#define PIT_CHAN1_DATA 0x41
#define PIT_CHAN2_DATA 0x42
#define PIT_COMMAND    0x43

#define PIT_SEL_CHAN0 0x00
#define PIT_SEL_CHAN1 0x40
#define PIT_SEL_CHAN2 0x80
#define PIT_SEL_READ  0xc0

#define PIT_ACC_LATCH 0x00
#define PIT_ACC_LO    0x10
#define PIT_ACC_HI    0x20
#define PIT_ACC_LOHI  0x30

#define PIT_MODE_ITC  0x00
#define PIT_MODE_TOS  0x02
#define PIT_MODE_RGN  0x04
#define PIT_MODE_SQG  0x06
#define PIT_MODE_STS  0x08
#define PIT_MODE_HTS  0x0A

#define PIC_DATA_BIN  0x00
#define PIC_DATA_BCD  0x01

void pit_stage_4_init();

#endif
