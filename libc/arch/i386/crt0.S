.section .text.startup

        .extern main
        .extern exit
        .extern _init
        .extern _argc
        .extern _argv
        .global _start
_start:
        # set up the end of the linked stack frame list:
        push $0
        push $0
        mov %esp, %ebp
 
	# Prepare signals, memory allocation, stdio and such.
	call _hellbender_libc_init_basic
	call _hellbender_libc_init_advanced

        # run global constructors, the application, and exit.
        call _init

	# Run main (with argc and argv).
        pushl (_argv)
        pushl (_argc)
        call main

	# Terminate the process with the exit code.
        push %eax
        call exit
        
