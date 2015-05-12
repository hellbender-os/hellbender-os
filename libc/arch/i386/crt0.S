.section .text.startup

        .extern main
        .extern exit
        .extern _init
        .global _start
_start:
        # set up the end of the linked stack frame list:
        push $0
        push $0
        mov %esp, %ebp
        
	# We need those in a moment when we call main.
	push %esi
	push %edi
 
	# Prepare signals, memory allocation, stdio and such.
	call _hellbender_libc_init

        # run global constructors, the application, and exit.
        call _init

	# Run main (with argc and argv).
        call main

	# Terminate the process with the exit code.
        push %eax
        call exit
        
