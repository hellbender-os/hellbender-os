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
        mov %esp, %ebx        // stack pointer in a callee save register.
        mov %esp, %eax        // pass (optional) ISR stack pointer parameter.
        add $8*4, %eax        // skip the values pushed a above.
        push %eax
        call isr_routine_\IDX
        pop %eax              // rewind stack.
        mov %ebx, %esp        // restore stack, in case parameter was not used.
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

