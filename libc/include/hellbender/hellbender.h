#ifndef _HELLBENDER_H
#define _HELLBENDER_H

#include <kernel/syscall.h>
#include <sys/types.h>
#include <stdint.h>

#ifndef BREAK
extern unsigned long __force_order;
#define BREAK { asm volatile("xchg %%bx, %%bx" : "=m"(__force_order)); }
#endif

#ifndef BARRIER
#define BARRIER asm volatile("": : :"memory");
#endif

#endif
