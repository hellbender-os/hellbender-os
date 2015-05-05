.section .text

        .extern main
        .extern exit
        .extern _init
        .global _start
_start:
        jmp _start1

        # standard hellbender program header located at _start + 8
        .org 8
        .long _hellbender_module_bottom
        .long _hellbender_module_top
        .long _hellbender_text_bottom
        .long _hellbender_text_top

        .align 4
_start1:        
        # set up the end of the linked stack frame list:
        push $0
        push $0
        mov %esp, %ebp

        # run global constructors, the application, and exit.
        call _init
        call main
        push %eax
        call exit
        
