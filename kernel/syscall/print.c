#include <stdio.h>
#include <kernel/syscall.h>

void syscall_print(const char* str) {
  // TODO: limit string length, don't write out of screen.
  printf(str);
}
