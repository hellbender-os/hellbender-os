	.file	"syscall_size.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.section	.text.unlikely,"ax",@progbits
.LCOLDB0:
	.section	.text.startup,"ax",@progbits
.LHOTB0:
	.align 16
	.section	.text.unlikely
.Ltext_cold0:
	.section	.text.startup
	.globl	main
	.type	main, @function
main:
.LFB2:
	.file 1 "syscall/syscall_size.c"
	.loc 1 46 0
	.cfi_startproc
	rep ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.section	.text.unlikely
.LCOLDE0:
	.section	.text.startup
.LHOTE0:
	.comm	_syscall_yield__size,0,1
	.comm	_syscall_get_environment__size,12,1
	.comm	_syscall_current_exec_path__size,4,1
	.comm	_syscall_spawn__size,12,1
	.comm	_syscall_exit__size,4,1
	.comm	_syscall_make_executable__size,8,1
	.comm	_syscall_make_readonly__size,8,1
	.comm	_syscall_set_program_break__size,8,1
	.comm	_syscall_print__size,4,1
	.text
.Letext0:
	.section	.text.unlikely
.Letext_cold0:
	.file 2 "/media/host/hellbender-os/sysroot/usr/include/stdint.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x255
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF31
	.byte	0x1
	.long	.LASF32
	.long	.LASF33
	.long	.Ldebug_ranges0+0
	.long	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF1
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF2
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF3
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.long	.LASF4
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF5
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF6
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF7
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF8
	.uleb128 0x4
	.long	.LASF34
	.byte	0x2
	.byte	0x53
	.long	0x48
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF9
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x6
	.byte	0x4
	.byte	0x1
	.byte	0x4
	.long	0x94
	.uleb128 0x7
	.string	"str"
	.byte	0x1
	.byte	0x5
	.long	0x94
	.byte	0
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.long	0x9a
	.uleb128 0x9
	.long	0x9f
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF10
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0x8
	.long	0xc7
	.uleb128 0xa
	.long	.LASF11
	.byte	0x1
	.byte	0x9
	.long	0x7d
	.byte	0
	.uleb128 0xa
	.long	.LASF12
	.byte	0x1
	.byte	0xa
	.long	0x6b
	.byte	0x4
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0xd
	.long	0xe8
	.uleb128 0xa
	.long	.LASF13
	.byte	0x1
	.byte	0xe
	.long	0x7d
	.byte	0
	.uleb128 0xa
	.long	.LASF14
	.byte	0x1
	.byte	0xf
	.long	0x2c
	.byte	0x4
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.byte	0x1
	.byte	0x12
	.long	0x109
	.uleb128 0xa
	.long	.LASF13
	.byte	0x1
	.byte	0x13
	.long	0x7d
	.byte	0
	.uleb128 0xa
	.long	.LASF14
	.byte	0x1
	.byte	0x14
	.long	0x2c
	.byte	0x4
	.byte	0
	.uleb128 0x6
	.byte	0x4
	.byte	0x1
	.byte	0x17
	.long	0x11e
	.uleb128 0xa
	.long	.LASF15
	.byte	0x1
	.byte	0x18
	.long	0x25
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0xc
	.byte	0x1
	.byte	0x1b
	.long	0x14b
	.uleb128 0xa
	.long	.LASF16
	.byte	0x1
	.byte	0x1c
	.long	0x94
	.byte	0
	.uleb128 0xa
	.long	.LASF17
	.byte	0x1
	.byte	0x1d
	.long	0x14b
	.byte	0x4
	.uleb128 0xa
	.long	.LASF18
	.byte	0x1
	.byte	0x1e
	.long	0x14b
	.byte	0x8
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.long	0x151
	.uleb128 0x9
	.long	0x156
	.uleb128 0x8
	.byte	0x4
	.long	0x9f
	.uleb128 0x6
	.byte	0x4
	.byte	0x1
	.byte	0x21
	.long	0x171
	.uleb128 0xa
	.long	.LASF16
	.byte	0x1
	.byte	0x22
	.long	0x156
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0xc
	.byte	0x1
	.byte	0x25
	.long	0x19e
	.uleb128 0xa
	.long	.LASF19
	.byte	0x1
	.byte	0x26
	.long	0x19e
	.byte	0
	.uleb128 0xa
	.long	.LASF20
	.byte	0x1
	.byte	0x27
	.long	0x19e
	.byte	0x4
	.uleb128 0xa
	.long	.LASF21
	.byte	0x1
	.byte	0x28
	.long	0x1a4
	.byte	0x8
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.long	0x25
	.uleb128 0x8
	.byte	0x4
	.long	0x156
	.uleb128 0xb
	.byte	0
	.byte	0x1
	.byte	0x2b
	.uleb128 0xc
	.long	.LASF35
	.byte	0x1
	.byte	0x2e
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0xd
	.long	.LASF22
	.byte	0x1
	.byte	0x6
	.long	0x7f
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_print__size
	.uleb128 0xd
	.long	.LASF23
	.byte	0x1
	.byte	0xb
	.long	0xa6
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_set_program_break__size
	.uleb128 0xd
	.long	.LASF24
	.byte	0x1
	.byte	0x10
	.long	0xc7
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_make_readonly__size
	.uleb128 0xd
	.long	.LASF25
	.byte	0x1
	.byte	0x15
	.long	0xe8
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_make_executable__size
	.uleb128 0xd
	.long	.LASF26
	.byte	0x1
	.byte	0x19
	.long	0x109
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_exit__size
	.uleb128 0xd
	.long	.LASF27
	.byte	0x1
	.byte	0x1f
	.long	0x11e
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_spawn__size
	.uleb128 0xd
	.long	.LASF28
	.byte	0x1
	.byte	0x23
	.long	0x15c
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_current_exec_path__size
	.uleb128 0xd
	.long	.LASF29
	.byte	0x1
	.byte	0x29
	.long	0x171
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_get_environment__size
	.uleb128 0xd
	.long	.LASF30
	.byte	0x1
	.byte	0x2c
	.long	0x1aa
	.uleb128 0x5
	.byte	0x3
	.long	_syscall_yield__size
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
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
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
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x3
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
	.uleb128 0x4
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
	.uleb128 0xd
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
	.long	.LFB2
	.long	.LFE2-.LFB2
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.LFB2
	.long	.LFE2
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF28:
	.string	"_syscall_current_exec_path__size"
.LASF20:
	.string	"envc"
.LASF21:
	.string	"strings"
.LASF18:
	.string	"envp"
.LASF31:
	.string	"GNU C 4.9.2 -mtune=generic -march=pentiumpro -g -O2 -std=gnu11 -ffreestanding -fbuiltin -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables"
.LASF3:
	.string	"short int"
.LASF34:
	.string	"intptr_t"
.LASF22:
	.string	"_syscall_print__size"
.LASF30:
	.string	"_syscall_yield__size"
.LASF35:
	.string	"main"
.LASF27:
	.string	"_syscall_spawn__size"
.LASF5:
	.string	"long long int"
.LASF33:
	.string	"/media/host/hellbender-os/kernel"
.LASF4:
	.string	"long int"
.LASF13:
	.string	"address"
.LASF29:
	.string	"_syscall_get_environment__size"
.LASF24:
	.string	"_syscall_make_readonly__size"
.LASF6:
	.string	"unsigned char"
.LASF19:
	.string	"argc"
.LASF2:
	.string	"signed char"
.LASF8:
	.string	"long long unsigned int"
.LASF25:
	.string	"_syscall_make_executable__size"
.LASF0:
	.string	"unsigned int"
.LASF17:
	.string	"argv"
.LASF32:
	.string	"syscall/syscall_size.c"
.LASF15:
	.string	"status"
.LASF7:
	.string	"short unsigned int"
.LASF12:
	.string	"delta_or_zero"
.LASF10:
	.string	"char"
.LASF1:
	.string	"long unsigned int"
.LASF14:
	.string	"size"
.LASF26:
	.string	"_syscall_exit__size"
.LASF16:
	.string	"path"
.LASF9:
	.string	"sizetype"
.LASF23:
	.string	"_syscall_set_program_break__size"
.LASF11:
	.string	"set_or_null"
	.ident	"GCC: (GNU) 4.9.2"
