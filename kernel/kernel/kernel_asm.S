        .section .text
        .extern kernel_in_ring3
        .global kernel_enter_ring3
kernel_enter_ring3:
        pop %eax // return address not needed
        pop %eax // data selector
        pop %edx // stack address
        pop %ebx // code selector
        pop %ecx // code address
        cli
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs
        push %eax  // SS
        push %edx // ESP
        pushf
        push %ebx  // CS
        push %ecx // EIP
        iret
