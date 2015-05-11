# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.extern kernel_stack
        
# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $(kernel_stack+16384+4096-8), %esp
    
	# Kernel initialization stages are chained together, never to return.
	push %ebx
	push %eax
	call early_stage_1

	# Hang if we still (unexpectedly) return.
.Lhang:
	cli
	hlt
	jmp .Lhang
.size _start, . - _start
