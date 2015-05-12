#ifndef _HELLBENDER_H
#define _HELLBENDER_H

#include <sys/types.h>

extern unsigned long __force_order;
#define BREAK { asm volatile("xchg %%bx, %%bx" : "=m"(__force_order)); }

void syscall_print(const char* str);
void syscall_allocate(void* address, unsigned size);

__attribute__((__noreturn__)) void syscall_exit(int status);
__attribute__((__noreturn__)) void syscall_iret(void);

typedef struct domain domain_t;
unsigned syscall_current_domain(domain_t *domain);

void syscall_yield();

typedef struct sem sem_t;
int syscall_sem_open(sem_t *s, const char* name, int oflag);
int syscall_sem_create(sem_t *s, const char* name, int oflag,
                       mode_t mode, unsigned value);
int syscall_sem_post(sem_t *s);
int syscall_sem_wait(sem_t *s);

#endif
