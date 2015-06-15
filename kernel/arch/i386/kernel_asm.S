        .section .text
        .global kernel_enter_ring0
        .global kernel_enter_ring3
        .global kernel_restore_context
        .extern _thread_kdata
        
kernel_enter_ring0: // use when current thread state == NEW; for kernel threads.
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
        mov %ax, %ss // ring0->ring0 iret won't load new stack.
        mov %edx, %esp
        pushf
        push %ebx  // CS
        push %ecx // EIP
        orl $0x200, 8(%esp)    // enable interrupts after iret
        iret
        
kernel_enter_ring3: // use when current thread state == NEW: for user threads.
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
        orl $0x200, 8(%esp)    // enable interrupts after iret
        iret

kernel_restore_context: // use when current thread state == OLD.
        //xchg %bx,%bx
        mov (_thread_kdata), %eax  // current thread structure is there.
        mov %eax, %esp        // restore the thread kernel stack pointer.

        // restore the context.
        pop %gs
        pop %fs
        pop %es
        pop %ds
        popal
        orl $0x200, 8(%esp)    // enable interrupts after iret
        iret
