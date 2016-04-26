        .section .text
        .code64
        .global syscall_resolve
syscall_resolve:
        .set rax_mask, 63 // must 2^n - 1
        and $rax_mask, %rax
        movabs $table_begin, %r10
        mov (%r10,%rax,8), %rax
        ret

table_begin:
        #include "syscall_resolve.inc"
table_end:
	.rept rax_mask + 1 - (table_end - table_begin) / 8
	.quad syscall_unknown
	.endr
