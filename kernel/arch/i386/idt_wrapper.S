        .section .text
        .extern idt_routines
        .global idt_wrapper_50
        .balign 4
idt_wrapper_50:
        pushal
        cld
        movl $(idt_routines+0x50*4), %edx 
        mov (%edx), %edx
        call *%edx
        popal
        iret
        
