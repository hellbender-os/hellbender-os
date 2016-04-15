#include "config.h"

.section .boot32
.code32

# The kernel entry point.
.equ boot_32, boot_32_himem - KERNEL_OFFSET
.global boot_32
.type boot_32, @function
boot_32_himem:
    # Will pass multiboot info to boot_64 function.
    movl %eax, %edi
    movl %ebx, %esi

    # Set the recursive page table hierarcy.
    movl $(page_ml4t - KERNEL_OFFSET), %eax
    movl %eax, %ebx
    add $(PAGE_SIZE - 8), %ebx
    movl %eax, (%ebx)
    orl $3, (%ebx)

    # Populate the identity mapping page tables.
    movl $(page_ml4t - KERNEL_OFFSET), %ebx
    movl $(page_dpt - KERNEL_OFFSET), %eax
    movl %eax, (%ebx) # first level 4 table entry to linearly map first 1GB.
    orl $0x3, (%ebx)
    add $((KERNEL_OFFSET & 0xFFFFFFFFFFFF) / 0x8000000000*8), %ebx
    movl %eax, (%ebx) # and this is the higher half (global) mapping of the same memory.
    orl $0x103, (%ebx)
    movl $(page_dir - KERNEL_OFFSET), %ebx
    movl %ebx, (%eax) # directory pointer table points to page directory.
    orl $103, (%eax)
    movl $0x183, %edx
    movl $512, %ecx
.SetEntry2M:
    mov %edx, (%ebx) # fill page directory with 2MB pages.
    add $0x200000, %edx
    add $8, %ebx
    loop .SetEntry2M
    
    # Set the PAE enable bit in CR4.
    movl %cr4, %eax
    orl $(1<<5), %eax
    movl %eax, %cr4
    
    # Load CR3 with the physical address of the PML4 table.
    movl $(page_ml4t - KERNEL_OFFSET), %eax
    movl %eax, %cr3
    
    # Enable long mode by setting the EFER.LME flag in MSR 0xC0000080
    movl $0xc0000080, %ecx
    rdmsr
    orl $(1<<8), %eax
    wrmsr
    
    # Enable paging. First 1GB is identity mapped so nothing happens for us.
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    # Set up GDT
    movl $(gdt_ptr - KERNEL_OFFSET), %ebx
    movl $(gdt - KERNEL_OFFSET), %eax
    movl %eax, 2(%ebx) # GDT pointer needs to be set, size is already there.
    lgdt gdt_ptr - KERNEL_OFFSET
    
    # Load selectors.
    mov $(DATA_SELECTOR), %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    # Long jump to enter long mode
    ljmp $8, $(boot_32_jump - KERNEL_OFFSET)

boot_32_jump:
.code64
    # Set up stack for kernel initialization code.
    movabs $(cpu_stack + CPU_STACK_SIZE - 8), %rsp

    # Jump to boot code in the higher half.
    movabs $boot_64, %rax
    jmp *%rax

