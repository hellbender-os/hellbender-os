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
_hellbender_module_info2:       
