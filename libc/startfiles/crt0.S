#include <hellbender/debug.h>
#include "struct2asm.inc"
        
        .section .text.startup
        .code64
        .extern main
        .extern exit
        .extern _libc_init_thread
        .extern _libc_init_args
        .extern _init
        .extern _argc
        .extern _argv
        .extern _vga_base
        .global _start
        
_start:
        # DEBUG: Write 'e' to VGA buffer to notify we are in usermode..
        mov $0x0f65, %ax
        mov %ax, (_vga_base+0x10)(%rip)

        # Save the top of stack, argv (already in the stack).
        mov %rsp, -16(%rsp)
        sub $16, %rsp
        mov %rsp, %rbp

        # By convention, we get thread local information in registers rdi, rsi, rdx.
        # and thread id, process id in r8, r9 (see kernel/src/thread.c).
        # We pass these to _libc_init_thread.
        call _libc_init_thread

        # We set argv and stack for parameters to _libc_init:
        mov 8(%rsp), %rdi # 1st parameter is argv, just below the original top of stack.
        mov (%rsp), %rsi  # 2nd parameter is the original top of stack.
        
        # Set up the end of the linked stack frame list:
        movq $0, (%rsp)
        movq $0, 8(%rsp)
 
	# Parse argv, envp from the stack.
	call _libc_init_args
        mov %rax, %rbx # keep the returned struct libc pointer.

        # Run global constructors.
        call _init

	# Run main (with argc and argv).
        mov offsetof_libc_argc(%rbx), %rdi
        mov offsetof_libc_argv(%rbx), %rsi
        call main

	# Terminate the process with the exit code.
        mov %rax, %rsi
        call exit

        # Exit won't return, but still..
1:      hlt
        jmp 1b
