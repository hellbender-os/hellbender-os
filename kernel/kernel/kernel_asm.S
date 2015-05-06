        .section .text
        .extern kernel_in_ring3
        .global kernel_enter_ring3
kernel_enter_ring3: // use when current thread state == NEW.
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

kernel_return_ring3: // use when current thread state == OLD.
        mov (0x800000), %eax  // current thread structure is there.
        mov %eax, %esp        // restore the thread kernel stack pointer.

        // restore the context.
        pop %gs
        pop %fs
        pop %es
        pop %ds
        popal
        iret
