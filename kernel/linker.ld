/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(boot_32)

 /* This load address with ALIGN after .multiboot makes sure our kernel
    occupies the last page of the first page table.
    lomem_init assume that the page is unused to simplify initialization. */
KERNEL_LOAD = 2M - 4k;
KERNEL_OFFSET = 0xFFFF800000000000;

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
    /* Higher half kernel in use. */
    . = KERNEL_LOAD + KERNEL_OFFSET;
    
    /* Multiboot header + 32-bit bootstrapping must be before any code. */
    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_OFFSET)
    {
        *(.multiboot)
        . = ALIGN(4K);
        *(.boot32)
        *(.text)
        *(.rodata*)
        . = ALIGN(4K);
    }

    /* Initialized data. */
    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_OFFSET)
    {
        *(.data)
        . = ALIGN(4K);
    }

    /* Uninitialized data */
    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_OFFSET)
    {
        *(.bss)
        *(COMMON)
        . = ALIGN(4K);
    }	
    
    /DISCARD/ :
    {
        *(.*note*)
        *(.comment)
    }
}
