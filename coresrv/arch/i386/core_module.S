        .global _hellbender_module_org
        _hellbender_module_org = 0x7F800000;
        
        .section .text
        .global _start
        .global _hellbender_module_info
_start:
_hellbender_module_info:
        .long _hellbender_module_info_end - _hellbender_module_info
        # core service information structure.
        .long _module_start # where actual entry point is.
        .long init_stage    # pointer to init coordination variable.
        .long rtc_isr       # pointer to timer ISR function.
        .long ps2_chan1_isr # pointer to keyboard ISR function.
        .long ps2_chan2_isr # pointer to mouse ISR function.
        .long vga_buffer    # pointer to memory reserved for VGA buffer.
        .long 0             # pointer to the initrd buffer (from kernel).
        .long 0             # size of the initrd buffer.
        .long load_process  # pointer to ELF process loading code.
_hellbender_module_info_end:       


        .global _module_start
        .extern _hellbender_libc_init_basic
        .extern _hellbender_libc_init_advanced
        .extern core_initialize
        .extern core_main
_module_start:
        # Core service init has to be interleaved with libC init.
        
	call _hellbender_libc_init_basic
        call core_initialize
	call _hellbender_libc_init_advanced

        # run global constructors before the core_main.
        call _init
        call core_main

	# Terminate the process in case core_main returns.
        push %eax
        call exit
        
