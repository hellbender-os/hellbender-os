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
        pushal
        cld
        mov %esp, %eax        // pass (original) ISR stack pointer parameter.
        add $8*4, %eax
        mov %ds, %bx          // copy DS into other selector registers.
        push %es
        push %fs
        push %gs
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs
        mov %esp, %ebp        // stack pointer in a callee save register.
        push %eax
        call isr_routine_\IDX
        mov %ebp, %esp        // restore stack, in case parameter was not used.
        pop %gs
        pop %fs
        pop %es
        popal
        iret
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

