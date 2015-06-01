	.file	"syscall_size.c"
	.text
.Ltext0:
	.comm	_syscall_print__size,8,1
	.comm	_syscall_set_program_break__size,8,1
	.comm	_syscall_make_readonly__size,8,1
	.comm	_syscall_make_executable__size,8,1
	.comm	_syscall_exit__size,4,1
	.comm	_syscall_spawn__size,12,1
	.comm	_syscall_current_exec_path__size,4,1
	.comm	_syscall_get_environment__size,12,1
	.comm	_syscall_yield__size,0,1
	.comm	_syscall_wait_eq__size,8,1
	.comm	_syscall_wait_neq__size,8,1
	.comm	_syscall_notify__size,4,1
	.globl	main
	.type	main, @function
main:
.LFB2:
	.file 1 "syscall/syscall_size.c"
	.loc 1 61 0
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	.loc 1 61 0
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
.Letext0:
	.file 2 "/home/hellbender/opt/cross/lib/gcc/i686-hellbender/4.9.2/include/stddef.h"
	.file 3 "/media/host/hellbender-os/sysroot/usr/include/stdint.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x306
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF38
	.byte	0x1
	.long	.LASF39
	.long	.LASF40
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.long	.LASF0
	.uleb128 0x3
	.long	.LASF10
	.byte	0x2
	.byte	0xd4
	.long	0x37
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF1
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF2
	.uleb128 0x2
	.byte	0xc
	.byte	0x4
	.long	.LASF3
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF4
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.long	.LASF5
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF6
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF8
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF9
	.uleb128 0x3
	.long	.LASF11
	.byte	0x3
	.byte	0x53
	.long	0x25
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF12
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0x4
	.long	0xb0
	.uleb128 0x7
	.string	"str"
	.byte	0x1
	.byte	0x5
	.long	0xb0
	.byte	0
	.uleb128 0x7
	.string	"n"
	.byte	0x1
	.byte	0x6
	.long	0x2c
	.byte	0x4
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.long	0xb6
	.uleb128 0x9
	.long	0xbb
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF13
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0x9
	.long	0xe3
	.uleb128 0xa
	.long	.LASF14
	.byte	0x1
	.byte	0xa
	.long	0x88
	.byte	0
	.uleb128 0xa
	.long	.LASF15
	.byte	0x1
	.byte	0xb
	.long	0x7d
	.byte	0x4
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0xe
	.long	0x104
	.uleb128 0xa
	.long	.LASF16
	.byte	0x1
	.byte	0xf
	.long	0x88
	.byte	0
	.uleb128 0xa
	.long	.LASF17
	.byte	0x1
	.byte	0x10
	.long	0x76
	.byte	0x4
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0x13
	.long	0x125
	.uleb128 0xa
	.long	.LASF16
	.byte	0x1
	.byte	0x14
	.long	0x88
	.byte	0
	.uleb128 0xa
	.long	.LASF17
	.byte	0x1
	.byte	0x15
	.long	0x76
	.byte	0x4
	.byte	0
	.uleb128 0x6
	.byte	0x4
	.byte	0x1
	.byte	0x18
	.long	0x13a
	.uleb128 0xa
	.long	.LASF18
	.byte	0x1
	.byte	0x19
	.long	0x3e
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0xc
	.byte	0x1
	.byte	0x1c
	.long	0x167
	.uleb128 0xa
	.long	.LASF19
	.byte	0x1
	.byte	0x1d
	.long	0xb0
	.byte	0
	.uleb128 0xa
	.long	.LASF20
	.byte	0x1
	.byte	0x1e
	.long	0x167
	.byte	0x4
	.uleb128 0xa
	.long	.LASF21
	.byte	0x1
	.byte	0x1f
	.long	0x167
	.byte	0x8
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.long	0x16d
	.uleb128 0x9
	.long	0x172
	.uleb128 0x8
	.byte	0x4
	.long	0xbb
	.uleb128 0x6
	.byte	0x4
	.byte	0x1
	.byte	0x22
	.long	0x18d
	.uleb128 0xa
	.long	.LASF19
	.byte	0x1
	.byte	0x23
	.long	0x172
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0xc
	.byte	0x1
	.byte	0x26
	.long	0x1ba
	.uleb128 0xa
	.long	.LASF22
	.byte	0x1
	.byte	0x27
	.long	0x1ba
	.byte	0
	.uleb128 0xa
	.long	.LASF23
	.byte	0x1
	.byte	0x28
	.long	0x1ba
	.byte	0x4
	.uleb128 0xa
	.long	.LASF24
	.byte	0x1
	.byte	0x29
	.long	0x1c0
	.byte	0x8
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.long	0x3e
	.uleb128 0x8
	.byte	0x4
	.long	0x172
	.uleb128 0xb
	.byte	0
	.byte	0x1
	.byte	0x2c
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0x2f
	.long	0x1eb
	.uleb128 0x7
	.string	"obj"
	.byte	0x1
	.byte	0x30
	.long	0x1eb
	.byte	0
	.uleb128 0xa
	.long	.LASF25
	.byte	0x1
	.byte	0x31
	.long	0x76
	.byte	0x4
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.long	0x1f1
	.uleb128 0xc
	.long	0x76
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0x34
	.long	0x217
	.uleb128 0x7
	.string	"obj"
	.byte	0x1
	.byte	0x35
	.long	0x1eb
	.byte	0
	.uleb128 0xa
	.long	.LASF25
	.byte	0x1
	.byte	0x36
	.long	0x76
	.byte	0x4
	.byte	0
	.uleb128 0x6
	.byte	0x4
	.byte	0x1
	.byte	0x39
	.long	0x22c
	.uleb128 0x7
	.string	"obj"
	.byte	0x1
	.byte	0x3a
	.long	0x1eb
	.byte	0
	.byte	0
	.uleb128 0xd
	.long	.LASF41
	.byte	0x1
	.byte	0x3d
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0xe
	.long	.LASF26
	.byte	0x1
	.byte	0x7
	.long	0x91
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_print__size
	.uleb128 0xe
	.long	.LASF27
	.byte	0x1
	.byte	0xc
	.long	0xc2
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_set_program_break__size
	.uleb128 0xe
	.long	.LASF28
	.byte	0x1
	.byte	0x11
	.long	0xe3
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_make_readonly__size
	.uleb128 0xe
	.long	.LASF29
	.byte	0x1
	.byte	0x16
	.long	0x104
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_make_executable__size
	.uleb128 0xe
	.long	.LASF30
	.byte	0x1
	.byte	0x1a
	.long	0x125
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_exit__size
	.uleb128 0xe
	.long	.LASF31
	.byte	0x1
	.byte	0x20
	.long	0x13a
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_spawn__size
	.uleb128 0xe
	.long	.LASF32
	.byte	0x1
	.byte	0x24
	.long	0x178
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_current_exec_path__size
	.uleb128 0xe
	.long	.LASF33
	.byte	0x1
	.byte	0x2a
	.long	0x18d
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_get_environment__size
	.uleb128 0xe
	.long	.LASF34
	.byte	0x1
	.byte	0x2d
	.long	0x1c6
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_yield__size
	.uleb128 0xe
	.long	.LASF35
	.byte	0x1
	.byte	0x32
	.long	0x1ca
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_wait_eq__size
	.uleb128 0xe
	.long	.LASF36
	.byte	0x1
	.byte	0x37
	.long	0x1f6
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_wait_neq__size
	.uleb128 0xe
	.long	.LASF37
	.byte	0x1
	.byte	0x3b
	.long	0x217
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_notify__size
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x13
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x35
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF32:
	.string	"_syscall_current_exec_path__size"
.LASF23:
	.string	"envc"
.LASF24:
	.string	"strings"
.LASF21:
	.string	"envp"
.LASF31:
	.string	"_syscall_spawn__size"
.LASF5:
	.string	"short int"
.LASF10:
	.string	"size_t"
.LASF11:
	.string	"intptr_t"
.LASF26:
	.string	"_syscall_print__size"
.LASF34:
	.string	"_syscall_yield__size"
.LASF41:
	.string	"main"
.LASF37:
	.string	"_syscall_notify__size"
.LASF25:
	.string	"value"
.LASF2:
	.string	"long long int"
.LASF40:
	.string	"/media/host/hellbender-os/kernel"
.LASF0:
	.string	"long int"
.LASF16:
	.string	"address"
.LASF33:
	.string	"_syscall_get_environment__size"
.LASF28:
	.string	"_syscall_make_readonly__size"
.LASF3:
	.string	"long double"
.LASF6:
	.string	"unsigned char"
.LASF22:
	.string	"argc"
.LASF4:
	.string	"signed char"
.LASF8:
	.string	"long long unsigned int"
.LASF29:
	.string	"_syscall_make_executable__size"
.LASF9:
	.string	"unsigned int"
.LASF20:
	.string	"argv"
.LASF39:
	.string	"syscall/syscall_size.c"
.LASF18:
	.string	"status"
.LASF7:
	.string	"short unsigned int"
.LASF15:
	.string	"delta_or_zero"
.LASF13:
	.string	"char"
.LASF36:
	.string	"_syscall_wait_neq__size"
.LASF1:
	.string	"long unsigned int"
.LASF17:
	.string	"size"
.LASF30:
	.string	"_syscall_exit__size"
.LASF19:
	.string	"path"
.LASF12:
	.string	"sizetype"
.LASF27:
	.string	"_syscall_set_program_break__size"
.LASF35:
	.string	"_syscall_wait_eq__size"
.LASF14:
	.string	"set_or_null"
.LASF38:
	.string	"GNU C 4.9.2 -mtune=generic -march=pentiumpro -g -O0 -std=gnu11 -ffreestanding -fbuiltin"
	.ident	"GCC: (GNU) 4.9.2"
