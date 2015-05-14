        // This file was automatically generated from coresrv.map
        .section .text

        .global coresrv_kbd_getc
coresrv_kbd_getc:
        mov $0x7f800000, %eax
        int $82

        .global coresrv_rtc_beep
coresrv_rtc_beep:
        mov $0x7f800008, %eax
        int $82

        .global coresrv_rtc_ticks
coresrv_rtc_ticks:
        mov $0x7f800010, %eax
        int $82

        .global coresrv_vga_putc
coresrv_vga_putc:
        mov $0x7f800018, %eax
        int $82
