.section .module_header

        # standard hellbender program header located at the beginning.
        .set MAGIC, 0x1337c0de
        .long MAGIC
        .long _hellbender_module_bottom
        .long _hellbender_module_top
        .long _hellbender_text_bottom
        .long _hellbender_text_top
        .long _start
        .long _hellbender_module_info
        .long _hellbender_checksum
