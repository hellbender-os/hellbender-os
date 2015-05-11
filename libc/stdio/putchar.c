#include <stdio.h>

#if defined(__is_hellbender_kernel)
#include <kernel/tty.h>

int putchar(int ic) {
  terminal_putchar(ic);
  return ic;
}

#else
#include <hellbender.h>

int putchar(int ic) {
  char str[2] = { (char)ic, '\0' };
  syscall_print(str);
  return ic;
}

#endif
