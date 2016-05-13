// Automagically generated my Makefile, do not edit.
#define SYSCALL_INT 80

.global syscall_exit
syscall_exit:
  mov $(1-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_set_signal_mask
syscall_set_signal_mask:
  mov $(2-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_set_irq_signal
syscall_set_irq_signal:
  mov $(3-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_wait_signal
syscall_wait_signal:
  mov $(4-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_register_service
syscall_register_service:
  mov $(5-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_alloc_tables
syscall_alloc_tables:
  mov $(6-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_log
syscall_log:
  mov $(7-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_create_thread
syscall_create_thread:
  mov $(8-1), %rax
  int $SYSCALL_INT
  ret

.global syscall_unknown
syscall_unknown:
  mov $(9-1), %rax
  int $SYSCALL_INT
  ret
