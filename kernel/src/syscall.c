#include "kernel.h"
#include "vga.h"

#include <stdint.h>

uint64_t syscall_exit(int retval) {
  (void)retval;
  // TODO: actual exit
  VGA_AT(0,10) = VGA_ENTRY('!', WHITE_ON_BLACK);
  kernel_panic();
}

// this should be last.
uint64_t syscall_unknown() {
  // TODO: kill the process?
  kernel_panic();
}
