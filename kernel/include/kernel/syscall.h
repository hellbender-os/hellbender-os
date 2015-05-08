#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#define SYSCALL "int $80"
#define SYSCALL_PRINT 0x01
#define SYSCALL_ALLOC 0x02
#define SYSCALL_EXIT  0xfe

#define SYSIRET "int $81"

#endif
