#include <kernel/pit.h>
#include <kernel/io.h>

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

void pit_initialize() {
  outb(PIT_COMMAND, PIT_SEL_CHAN0 | PIT_ACC_LOHI | PIT_MODE_RGN);
  outb(PIT_CHAN0_DATA, 0xff);
  outb(PIT_CHAN0_DATA, 0xff);
}
