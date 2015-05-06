#include <kernel/isr.h>
#include <kernel/syscall.h>

void syscall_print(const char* str) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_PRINT), "d"(str));
}

__attribute__((__noreturn__)) void syscall_exit(int status) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_EXIT), "d"(status));
  __builtin_unreachable();
}

__attribute__((__noreturn__)) void syscall_iret(void) {
  asm(SYSIRET : "=m"(__force_order));
  __builtin_unreachable();
}
