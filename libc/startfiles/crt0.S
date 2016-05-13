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
        # Kernel passes libc_init structure in rdi, that is the parameter to _libc_init

        # Align stack and set up the end of the linked stack frame list:
        and $-16, %rsp
        pushq $0
        pushq $0

        # Initialize the libc (parses argv, envp, etc.)
        call _libc_init
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
