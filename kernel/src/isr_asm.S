        #include "config.h"
        #include "struct2asm.inc"

        .section .text
        .code64

        // errors are interrupts that push an error code into the stack:
        .altmacro
        .macro define_error IDX
        __define_error %IDX
        .endm

        .macro __define_error IDX
        .extern isr_routine_\IDX
        .global isr_wrapper_\IDX
        .balign 4
isr_wrapper_\IDX:
        sub $(sizeof_thread_registers), %rsp
        mov %rax, offsetof_thread_registers_rax(%rsp)
        movabs $isr_routine_\IDX, %rax // the routine we will call to handle the interrup.
        jmp isr_push_state
        .endm

        
        // normal interrupts need to push a mock error code so that we have standard stack:
        .altmacro
        .macro define_interrupt IDX
        __define_interrupt %IDX
        .endm

        .macro __define_interrupt IDX
        .extern isr_routine_\IDX
        .global isr_wrapper_\IDX
        .balign 4
isr_wrapper_\IDX:
        pushq $0 // mock error code.
        sub $(sizeof_thread_registers), %rsp
        mov %rax, offsetof_thread_registers_rax(%rsp)
        movabs $isr_routine_\IDX, %rax // the routine we will call to handle the interrup.
        jmp isr_push_state
        .endm

        
        // common state saving / restoring:
isr_push_state:
        mov %rbx, offsetof_thread_registers_rbx(%rsp)
        mov %rcx, offsetof_thread_registers_rcx(%rsp)
        mov %rdx, offsetof_thread_registers_rdx(%rsp)
        mov %rbp, offsetof_thread_registers_rbp(%rsp)
        mov %rsi, offsetof_thread_registers_rsi(%rsp)
        mov %rdi, offsetof_thread_registers_rdi(%rsp)
        mov %r8, offsetof_thread_registers_r8(%rsp)
        mov %r9, offsetof_thread_registers_r9(%rsp)
        mov %r10, offsetof_thread_registers_r10(%rsp)
        mov %r11, offsetof_thread_registers_r11(%rsp)
        mov %r12, offsetof_thread_registers_r12(%rsp)
        mov %r13, offsetof_thread_registers_r13(%rsp)
        mov %r14, offsetof_thread_registers_r14(%rsp)
        mov %r15, offsetof_thread_registers_r15(%rsp)
        mov $DATA_SELECTOR, %bx
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs
        mov %rsp, %rdi // 1st parameter == struct thread_state*
        mov $0, %rsi   // 2nd parameter == source ring number (0 for kernel, 3 for userspace).
        // if (++cpu.nested_isr == 0) {
        movabs $cpu, %rbx
        incq offsetof_cpu_nested_isr(%rbx)
        jnz 1f
        mov $3, %rdi
        //    cpu.current_thread_rsp = rsp;
        mov %rsp, offsetof_cpu_current_thread_rsp(%rbx)
        //    rsp = cpu.kernel_stack_top
        mov offsetof_cpu_kernel_stack_top(%rbx), %rsp
1:      // }
        sti
        call *%rax
        .global isr_to_usermode
isr_to_usermode:         // this same code works when entering usermode the first time.
        cli
        // if (--cpu.nested_isr < 0) {
        movabs $cpu, %rbx
        decq offsetof_cpu_nested_isr(%rbx)
        jns 1f
        //   rsp = cpu.current_thread_rsp
        mov offsetof_cpu_current_thread_rsp(%rbx), %rsp
1:      // }
        mov $UDATA_SELECTOR, %bx
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs
        mov offsetof_thread_registers_rax(%rsp), %rax
        mov offsetof_thread_registers_rbx(%rsp), %rbx
        mov offsetof_thread_registers_rcx(%rsp), %rcx
        mov offsetof_thread_registers_rdx(%rsp), %rdx
        mov offsetof_thread_registers_rbp(%rsp), %rbp
        mov offsetof_thread_registers_rsi(%rsp), %rsi
        mov offsetof_thread_registers_rdi(%rsp), %rdi
        mov offsetof_thread_registers_r8(%rsp), %r8
        mov offsetof_thread_registers_r9(%rsp), %r9
        mov offsetof_thread_registers_r10(%rsp), %r10
        mov offsetof_thread_registers_r11(%rsp), %r11
        mov offsetof_thread_registers_r12(%rsp), %r12
        mov offsetof_thread_registers_r13(%rsp), %r13
        mov offsetof_thread_registers_r14(%rsp), %r14
        mov offsetof_thread_registers_r15(%rsp), %r15
        add $(sizeof_thread_registers + 8), %rsp // also popping the error code.
        iretq


        // traps               Name	                 Vector	Type	Mnemonic Error code
        define_interrupt 0  // Divide-by-zero Error           0 (0x0)	Fault	#DE	No
        define_interrupt 1  // Debug                          1 (0x1)	Flt/Trp	#DB	No
        define_interrupt 2  // Non-maskable Interrupt         2 (0x2)	Interpt	-	No
        define_interrupt 3  // Breakpoint                     3 (0x3)	Trap	#BP	No
        define_interrupt 4  // Overflow                       4 (0x4)	Trap	#OF	No
        define_interrupt 5  // Bound Range Exceeded           5 (0x5)	Fault	#BR	No
        define_interrupt 6  // Invalid Opcode                 6 (0x6)	Fault	#UD	No
        define_interrupt 7  // Device Not Available           7 (0x7)	Fault	#NM	No
        define_error     8  // Double Fault                   8 (0x8)	Abort	#DF	Yes (Zero)
        define_interrupt 9  // Coprocessor Segment Overrun    9 (0x9)	Fault	-	No
        define_error     10 // Invalid TSS                   10 (0xA)	Fault	#TS	Yes
        define_error     11 // Segment Not Present           11 (0xB)	Fault	#NP	Yes
        define_error     12 // Stack-Segment Fault           12 (0xC)	Fault	#SS	Yes
        define_error     13 // General Protection Fault      13 (0xD)	Fault	#GP	Yes
        define_error     14 // Page Fault                    14 (0xE)	Fault	#PF	Yes
        define_interrupt 15 // Reserved                      15 (0xF)	-	-	No
        define_interrupt 16 // x87 Floating-Point Exception  16 (0x10)	Fault	#MF	No
        define_error     17 // Alignment Check               17 (0x11)	Fault	#AC	Yes
        define_interrupt 18 // Machine Check                 18 (0x12)	Abort	#MC	No
        define_interrupt 19 // SIMD Floating-Point Exception 19 (0x13)	Fault	#XM/#XF	No
        define_interrupt 20 // Virtualization Exception      20 (0x14)	Fault	#VE	No
        define_error     30 // Security Exception            30 (0x1E)	-	#SX	Yes

        // PIC interrupts
        .set idx, 32
        .rept 16
        define_interrupt idx
        .set idx, (idx+1)
        .endr

        
        // Syscall
        .macro __define_syscall INT
        .extern dispatch
        .global isr_wrapper_\INT
isr_wrapper_\INT:
        pushq %rax // syscall number as error code.
        sub $(sizeof_thread_registers), %rsp
        mov %rbx, offsetof_thread_registers_rbx(%rsp)
        mov %rbp, offsetof_thread_registers_rbp(%rsp)
        mov %r12, offsetof_thread_registers_r12(%rsp)
        mov %r13, offsetof_thread_registers_r13(%rsp)
        mov %r14, offsetof_thread_registers_r14(%rsp)
        mov %r15, offsetof_thread_registers_r15(%rsp)
        mov $DATA_SELECTOR, %bx
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs
        movabs $cpu, %rbx
        incq offsetof_cpu_nested_isr(%rbx)
        //    cpu.current_thread_rsp = rsp;
        mov %rsp, offsetof_cpu_current_thread_rsp(%rbx)
        //    rsp = cpu.kernel_stack_top
        mov offsetof_cpu_kernel_stack_top(%rbx), %rsp
        sti
        call dispatch
        cli
        movabs $cpu, %rbx
        decq offsetof_cpu_nested_isr(%rbx)
        //   rsp = cpu.current_thread_rsp
        mov offsetof_cpu_current_thread_rsp(%rbx), %rsp
        mov $UDATA_SELECTOR, %bx
        mov %bx, %ds
        mov %bx, %es
        mov %bx, %fs
        mov %bx, %gs
        mov offsetof_thread_registers_rbx(%rsp), %rbx
        mov offsetof_thread_registers_rbp(%rsp), %rbp
        mov offsetof_thread_registers_r12(%rsp), %r12
        mov offsetof_thread_registers_r13(%rsp), %r13
        mov offsetof_thread_registers_r14(%rsp), %r14
        mov offsetof_thread_registers_r15(%rsp), %r15
        add $(sizeof_thread_registers + 8), %rsp // also popping the error code.
        iretq
        .endm
        __define_syscall SYSCALL_INT
