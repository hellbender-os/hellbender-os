#ifndef _HELLBENDER_H
#define _HELLBENDER_H

void syscall_print(const char* str);

__attribute__((__noreturn__)) void syscall_exit(int status);
__attribute__((__noreturn__)) void syscall_iret(void);

#endif
