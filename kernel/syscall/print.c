#include <stdio.h>
#include <kernel/syscall.h>

void syscall_print(const char* str, size_t n) {
  for (size_t i = 0; i < n; i++) {
    putchar((int) ((const unsigned char*) str)[i]);
  }
}
