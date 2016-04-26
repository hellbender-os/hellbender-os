#include "cpu.h"
#include "thread.h"
#include <stdint.h>

/* magic macros -- don't change these */
#define OFFSETOF(TYPE, MEMBER) ((uintptr_t) &((struct TYPE *)0)->MEMBER)
#define SIZEOF(TYPE, MEMBER) (sizeof(((struct TYPE *)0)->MEMBER))
 
#define _DEFINE(sym, val) asm volatile("\n-> " #sym " %0 " #val "\n" : : "i" (val))
#define STRUCT(s) \
	_DEFINE(sizeof_##s, sizeof(struct s));
#define MEMBER(s, m) \
	_DEFINE(offsetof_##s##_##m, OFFSETOF(s, m)); \
	_DEFINE(sizeof_##s##_##m, SIZEOF(s, m));
 
/* function with your structures and members */
foo() {
  STRUCT(thread_context);

  STRUCT(thread_registers);
  MEMBER(thread_registers, tmp);
  MEMBER(thread_registers, rax);
  MEMBER(thread_registers, rbx);
  MEMBER(thread_registers, rcx);
  MEMBER(thread_registers, rdx);
  MEMBER(thread_registers, rbp);
  MEMBER(thread_registers, rsi);
  MEMBER(thread_registers, rdi);
  MEMBER(thread_registers, r8);
  MEMBER(thread_registers, r9);
  MEMBER(thread_registers, r10);
  MEMBER(thread_registers, r11);
  MEMBER(thread_registers, r12);
  MEMBER(thread_registers, r13);
  MEMBER(thread_registers, r14);
  MEMBER(thread_registers, r15);

  STRUCT(thread_state);
  MEMBER(thread_state, context);
  MEMBER(thread_state, registers);

  STRUCT(cpu);
  MEMBER(cpu, nested_isr);
  MEMBER(cpu, kernel_stack_top);
  MEMBER(cpu, current_thread);
  MEMBER(cpu, current_thread_rsp);
}
