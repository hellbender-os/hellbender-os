        .section .text

        .altmacro
        .macro define_wrapper IDX
        __define_wrapper %IDX
        .endm

        // ISR to use before system is ready for real action.
        .extern isr_debugger
        .global isr_wrapper_debug
        .balign 4
isr_wrapper_debug:
        // save the state for debugging
        xchg %bx,%bx
        pushal
        push %ds
        push %es
        push %fs
        push %gs
        mov %esp, %eax        // remember the thread kernel stack pointer.
        
        // we can use the botton of the stack in this emergency.
	movl $(kernel_stack+2048+4096-8), %esp

        // and then we jump to the debugging routine.
        push %eax             // 2nd parameter: all registers.
        add $(12*4), %eax
        push %eax             // 1st parameter: interrupt specific
        cld
        push $0               // fake return address because we don't live this.
        jmp isr_debugger
        
        
        // Context saving ISR wrappers.
        .macro __define_wrapper IDX
        .extern isr_routine_\IDX
        .global isr_wrapper_\IDX
        .balign 4
isr_wrapper_\IDX\():
        // save the state to active thread kernel stack.
        //xchg %bx,%bx
        pushal
        push %ds
        push %es
        push %fs
        push %gs
        mov %esp, %eax        // remember the thread kernel stack pointer.
        mov %eax, (0x400000)  // current thread structure is there.
        
        // now the context has been saved. time to switch to kernel stack.
	movl $(kernel_stack+16384+4096-8), %esp
        mov %ss, %bx          // copy kernel data selector into all registers.
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs

        // and then we jump to the interrupt service routine -routine.
        push %eax             // 2nd parameter: all registers.
        add $(12*4), %eax
        push %eax             // 1st parameter: interrupt specific
        cld
        push $0               // fake return address because
        jmp isr_routine_\IDX  // we use kernel_return_ring3 to go back.
        .endm


        // Context discarding ISR wrappers.
        .macro define_wrapper2 IDX
        .extern isr_routine_\IDX
        .global isr_wrapper_\IDX
        .balign 4
isr_wrapper_\IDX\():
        // don't save the state because we are not returning.
        //xchg %bx,%bx
	movl $(kernel_stack+16384+4096-12), %esp
        mov %ss, %bx          // copy kernel data selector into all registers.
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs

        // and then we jump to the interrupt service routine -routine.
        push %eax             // 1st parameter: interrupt specific
        cld
        push $0               // fake return address because
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
        define_wrapper 80
        define_wrapper2 81

