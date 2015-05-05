#include <kernel/isr.h>

void syscall() {
  isr_interrupt(80);
}
