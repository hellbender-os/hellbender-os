#include <hellbender.h>
#include <kernel/io.h>

void keyboard_isr() {
  char buffer[81];
  unsigned key = inb(0x60);
  sprintf(buffer, "Keyboard ISR: %x\n", key);
  syscall_print(buffer);
  syscall_iret();
}
