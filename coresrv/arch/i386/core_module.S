        .global _hellbender_module_org
        _hellbender_module_org = 0x7F800000 ;
        
        .section .text
        .global _hellbender_module_info
_hellbender_module_info:
        .long _hellbender_module_info2-_hellbender_module_info
        # core service isr functions.
        .long rtc_isr
        .long ps2_chan1_isr
        .long ps2_chan2_isr
        .long vga_buffer
        .long 0 # initrd buffer, filled by early kernel init.
        .long 0 # initrd size.
        .long load_process
_hellbender_module_info2:       


        .global _start
        .extern _hellbender_libc_init_basic
        .extern _hellbender_libc_init_advanced
        .extern core_initialize
        .extern core_main
_start:
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
        
