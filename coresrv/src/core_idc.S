        .section .idc_table
        .global core_get_ticks
idc_coresrv_get_ticks:
        calll coresrv_get_ticks //(5 bytes)
        pop %edx                //(1 bytes)
        int $83                 //(2 bytes)

        .balignl 4096, 0x53cd5a90
