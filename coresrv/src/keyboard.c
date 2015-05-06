#include <hellbender.h>

void keyboard_isr() {
  syscall_iret();
}
