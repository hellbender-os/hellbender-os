        .section .text

        .altmacro
        .macro define_wrapper IDX
        __define_wrapper %IDX
        .endm
        
        .macro __define_wrapper IDX
        .extern isr_routine_\IDX
        .global isr_wrapper_\IDX
        .balign 4
isr_wrapper_\IDX\():
        // save the state to active thread kernel stack.
        pushal
        push %ds
        push %es
        push %fs
        push %gs
        mov %esp, %eax        // remember the thread kernel stack pointer.
        mov %eax, (0x800000)  // current thread structure is there.
        
        // now the context has been saved. time to switch to kernel stack.
	movl $(kernel_stack+16384+4096), %esp
        mov %ss, %bx          // copy kernel data selector into all registers.
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs

        // and then we jump to the interrupt service routine -routine.
        add $(8*4+4*2), %eax  // pass the original ISR stack pointer.
        push %eax
        cld
        jmp isr_routine_\IDX  // we use kernel_return_ring3 to go back.
        .endm

        // protected mode traps
        .set idx, 0
        .rept 32
        define_wrapper idx
        .set idx, (idx+1)
        .endr

        // (re-mapped) master PIC interrupts
        .set idx, 32
        .rept 8
        define_wrapper idx
        .set idx, (idx+1)
        .endr

        // slave PIC interrups
        .set idx, 112
        .rept 8
        define_wrapper idx
        .set idx, (idx+1)
        .endr
        
        // kernel interrupts
        .set idx, 80
        .rept 8
        define_wrapper idx
        .set idx, (idx+1)
        .endr

