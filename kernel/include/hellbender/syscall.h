#ifndef __HELLBENDER_SYSCALL_H__
#define __HELLBENDER_SYSCALL_H__

__attribute__((__noreturn__))
void syscall_exit(int status) {
  //TODO: execute the call.
  asm("cli;1:hlt;jmp 1b");
  __builtin_unreachable();
}

#endif
