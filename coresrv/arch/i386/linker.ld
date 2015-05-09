OUTPUT_FORMAT("binary")

/* Entry point */
ENTRY(_start)

_hellbender_module_info = DEFINED(_hellbender_module_info) ? _hellbender_module_info : 0;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Core services are based at the end of our address space. */
	. = DEFINED(_hellbender_module_org) ? _hellbender_module_org : 0x800000;
        _hellbender_module_bottom = .;
        _hellbender_text_bottom = .;

	/* The module header must be first,
           followed by the standard .text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
                *(.module_header)
		*(.text)
	}

        _hellbender_text_top = .;

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) */
	.bss :
	{
		*(COMMON)
		*(.bss)
	}

        _hellbender_module_top = .;

        _hellbender_checksum = 0x1337 + 0xc0de + (_hellbender_module_bottom>>16) + (_hellbender_module_bottom&0xffff) + (_hellbender_module_top>>16) + (_hellbender_module_top&0xffff) + (_hellbender_text_bottom>>16) + (_hellbender_text_bottom&0xffff) + (_hellbender_text_top>>16) + (_hellbender_text_top&0xffff) + (_start>>16) + (_start&0xffff) + (_hellbender_module_info&0xffff) + (_hellbender_module_info>>16);
}
