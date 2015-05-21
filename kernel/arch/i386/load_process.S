        .section .bootstrap
        .global load_process
        .extern __IDE__elf_load_process

load_process:
        mov $__IDE__elf_load_process, %eax
        push %eax
        call do_idc_call // returns the address of _start
        jmp %eax

do_idc_call:
        pop %edx
        mov (%esp), %eax
        int $82;
