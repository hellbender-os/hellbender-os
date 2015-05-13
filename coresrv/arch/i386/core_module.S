        .global _hellbender_module_org
        _hellbender_module_org = 0x7F800000 ;
        
        .section .text
        .global _hellbender_module_info
_hellbender_module_info:
        .long _hellbender_module_info2-_hellbender_module_info
        # core service isr functions.
        .long kbd_isr
        .long rtc_isr
        .long vga_buffer
_hellbender_module_info2:       
