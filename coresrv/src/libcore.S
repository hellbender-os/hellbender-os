        .section .text
        // these should be (automatically) extracted from linker file.
        .set CORE_BASE, 0x7F800000
        .set idc_coresrv_get_ticks, CORE_BASE + 0

        .global coresrv_get_ticks
coresrv_get_ticks:
        mov $idc_coresrv_get_ticks, %eax
        int $82
        
