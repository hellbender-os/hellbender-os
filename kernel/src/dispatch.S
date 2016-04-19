        .section .text
        .code64
        .global dispatch
dispatch:
        .set rax_mask, 63 // must 2^n - 1
        and $rax_mask, %rax
        movabs $table_begin, %r10
        mov (%r10,%rax,8), %rax
        jmp *%rax

table_begin:
        #include "dispatch.inc"
table_end:
	.rept rax_mask + 1 - (table_end - table_begin) / 8
	.quad syscall_unknown
	.endr
