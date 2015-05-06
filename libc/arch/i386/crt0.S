.section .text

        .extern main
        .extern exit
        .extern _init
        .global _start
_start:
        # set up the end of the linked stack frame list:
        push $0
        push $0
        mov %esp, %ebp

        # run global constructors, the application, and exit.
        call _init
        call main
        push %eax
        call exit
        
