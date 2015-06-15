        .section .text
        .extern thread_calc_time
        .extern _thread_kdata

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
        //xchg %bx,%bx
        pushal
        push %ds
        push %es
        push %fs
        push %gs
        mov %esp, %eax        // remember the thread kernel stack pointer.
        
        // we can use the botton of the stack in this emergency.
	movl $(kernel_stack+2048+4096-8), %esp // -8 to respect stack boundary.

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
        mov %eax, (_thread_kdata)  // current thread structure is there.
        //cmp $0x401fbc, %eax
        //jz 1f
        //xchg %bx,%bx
        //1:      
        
        // now the context has been saved. time to switch to kernel stack.
	movl $(kernel_stack+16384+4096-8), %esp // -8 to respect stack boundary.
        mov %ss, %bx          // copy kernel data selector into all registers.
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs

        // and then we jump to the interrupt service routine -routine.
        push %eax             // 2nd parameter: all registers.
        add $(12*4), %eax
        push %eax             // 1st parameter: interrupt specific
        call thread_calc_time // now it's safe to update the thread exec time.
        cld
        push $0               // fake return address because
        jmp isr_routine_\IDX  // we use kernel_restore_context to go back.
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
        
        // Stack based syscall mechanism.
        .extern syscall_table
        .global isr_wrapper_80
        .balign 4
isr_wrapper_80:
        // TSS stack: ss, esp, flags, cs, eip
        // User stack: parameters, real_return
        // Syscall index in eax
        
        // save the state to active thread kernel stack.
        //xchg %bx,%bx
        pushal   // pushes 8 dwords
        push %ds // pushes 4 dwords
        push %es
        push %fs
        push %gs
        mov %esp, %ebp        // remember the thread kernel stack pointer.
        mov %ebp, (_thread_kdata)  // current thread structure is there.
        //cmp $0x401fbc, %ebp
        //jz 1f
        //xchg %bx,%bx
        //1:
        
        // now the context has been saved. time to switch to kernel stack.
	movl $(kernel_stack+16384+4096), %esp
        mov %ss, %bx          // copy kernel data selector into all registers.
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs

        // resolve user esp (depends on source ring)
        mov 13*4(%ebp), %edx // we check the code segment value.
        lea 15*4(%ebp), %esi // this has the ring 0 stack, or ring 3 esp value.
        and $3, %edx
        jz 1f
        mov (%esi), %esi     // now esi is user stack.
1:      
        // prepare for the syscall:
        mov $syscall_table, %edx
        and $0x3ff, %eax           // force to a valid syscall idx.
        mov 4(%edx, %eax, 8), %ecx // size of the stack call frame.
        mov 0(%edx, %eax, 8), %ebx // address of the syscall.

        // we copy parameters from user stack into kernel stack.
        // TODO: make sure user stack pointer is valid!

        add $4, %esi          // end of parameters.
        mov %ecx, %eax
        add $15, %eax
        and $0xfffffff0, %eax // eax is the size rounded to next 16 bytes.
        sub %eax, %esp        // make room for stack frame.
        mov %esp, %edi
        rep movsb

        // now it's good time to update the thread exec time.
        call thread_calc_time

        // and then we jump to the interrupt service routine -routine.
        cld
        call *%ebx            // we use kernel_to_usermode to go back
        mov %eax, 11*4(%ebp)  // but want to keep the return value.
        jmp kernel_to_usermode


        // IRET syscall doesn't save state because it's not needed.
        .extern isr_routine_81
        .global isr_wrapper_81
        .balign 4
isr_wrapper_81:
        // don't save the state because we are not returning.
        //xchg %bx,%bx
	movl $(kernel_stack+16384+4096-12), %esp // -12 for stack boundary.
        mov %ss, %bx          // copy kernel data selector into all registers.
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs

        // and then we jump to the interrupt service routine -routine.
        push %eax             // 1st parameter: interrupt specific
        call thread_calc_time // now it's safe to update the thread exec time.
        cld
        push $0               // fake return address because
        jmp isr_routine_81    // we use kernel_return_ring3 to go back.

        
        // Inter-domain call:
        .extern domain_push
        .global isr_wrapper_82
        .balign 4
isr_wrapper_82:
        // Target_address points to IDC wrapper at the first page of the domain.
        // eax: target_address; edx: return address
        // kernel stack: ss, esp, flags, cs, eip
        mov %eax, (%esp)
        sub $8, %esp // to respect stack boundary.
        push %edx
        push %eax
        call domain_push // checks that target_address is valid, enables domain.
        add $16, %esp // might be needed if we were called from ring0.
        // kernel stack: ss, esp, flags, cs, target_address
        iret // returns to ring3 at the target_address.

        
        // Inter-domain return:
        .global isr_wrapper_83
        .balign 4
isr_wrapper_83:
        // IDC wrapper has called target, time to return.
        // eax: return_value
        // stack: ss, esp, flags, cs, eip
        push %eax
        call domain_pop // disables domain, returns the original return address.
        mov %eax, 4(%esp)
        pop %eax
        // stack: ss, esp, flags, cs, return_address
        iret // returns to ring3 at the target_address.
