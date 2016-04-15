
# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# This section will be the first one in the kernel image.
.section .multiboot
        
# Declare a header as in the Multiboot Standard.
multiboot_header:
.long MAGIC
.long FLAGS
.long CHECKSUM
