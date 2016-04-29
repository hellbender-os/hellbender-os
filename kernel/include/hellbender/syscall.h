// Automagically generated my Makefile, do not edit.
#ifndef __HELLBENDER_SYSCALL_H__
#define __HELLBENDER_SYSCALL_H__
#include <stdint.h>
#define SYSCALL_INT 80
uint64_t syscall_exit(int retval) ;
uint64_t syscall_set_signal_mask(uint64_t blocked_mask) ;
uint64_t syscall_set_irq_signal(unsigned irqnum, int signum) ;
uint64_t syscall_wait_signal(uint64_t wait_mask) ;
uint64_t syscall_register_service(void* func) ;
uint64_t syscall_alloc_tables(uint64_t n_tables) ;
uint64_t syscall_unknown() ;
#endif
