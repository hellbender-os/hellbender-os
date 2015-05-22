#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#define SYSCALL "int $80"
#define SYSCALL_PRINT             0x01
#define SYSCALL_SET_PROGRAM_BREAK 0x02
#define SYSCALL_MAKE_READONLY     0x03
#define SYSCALL_MAKE_EXECUTABLE   0x04
#define SYSCALL_CURRENT_EXEC_PATH 0x05
#define SYSCALL_GET_ENVIRONMENT   0x06
#define SYSCALL_YIELD             0x07
#define SYSCALL_SPAWN             0x08

#define SYSCALL_EXIT              0xfe

#define SYSCALL_SEM_OPEN   0x10
#define SYSCALL_SEM_CREATE 0x11
#define SYSCALL_SEM_POST   0x12
#define SYSCALL_SEM_WAIT   0x13



#define SYSIRET "int $81"


#define IDCALL "int $82"
#define IDRET "int $83"

#endif
