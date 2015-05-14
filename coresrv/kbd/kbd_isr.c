#include <stdio.h>
#include <hellbender.h>
#include <kernel/io.h>

void kbd_isr() {
  char buffer[81];
  unsigned key = inb(0x60);
  printf("Keyboard ISR: %x\n", key);
  syscall_iret();
}
