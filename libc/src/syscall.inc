// Automagically generated my Makefile, do not edit.
#define SYSCALL_INT 80

.global syscall_exit
syscall_exit:
  mov $(1-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_unknown
syscall_unknown:
  mov $(2-1), %rax
  int $SYSCALL_INT
  ret
