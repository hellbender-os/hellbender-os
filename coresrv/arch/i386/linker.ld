OUTPUT_FORMAT("binary")

/* Entry point located at the beginning of the file. */
start = 0;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Core services are based at the end of our address space. */
	. = 2040M;
        _hellbender_module_bottom = .;
        _hellbender_text_bottom = .;

	/* The service entry code must be first, 
           followed by the standard .text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
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
}
