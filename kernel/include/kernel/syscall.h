#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <kernel/kernel.h>

#define SYSCALL "int $80"
#define SYSIRET "int $81"

#define IDCALL "int $82"
#define IDRET "int $83"

#define __SYS__

__SYS__ void syscall_print(const char* str, size_t n);
__SYS__ void* syscall_set_program_break(void* set_or_null, intptr_t delta_or_zero);
__SYS__ void syscall_make_readonly(void* address, unsigned size);
__SYS__ void syscall_make_executable(void* address, unsigned size);

__SYS__ __attribute__((__noreturn__)) void syscall_exit(int status);

__SYS__ int syscall_spawn(const char *path, char *const* argv, char *const* envp);
__SYS__ void syscall_current_exec_path(char* path);
__SYS__ void syscall_get_environment(int *argc, int *envc, char** strings);

__SYS__ void syscall_yield();

__SYS__ void syscall_wait_eq(volatile unsigned *obj, unsigned value);
__SYS__ void syscall_wait_neq(volatile unsigned *obj, unsigned value);
__SYS__ void syscall_notify(volatile unsigned *obj);

#define syscall_iret() \
  asm(SYSIRET : "=m"(__force_order)); \
  __builtin_unreachable();

#endif
