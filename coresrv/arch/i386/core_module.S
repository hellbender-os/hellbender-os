        .global _hellbender_module_org
        _hellbender_module_org = 0x7F800000 ;
        
        .section .text
        .global _hellbender_module_info
_hellbender_module_info:
        .long _hellbender_module_info2-_hellbender_module_info
        # core service isr functions.
        .long keyboard_isr
        .long timer_isr
_hellbender_module_info2:       
