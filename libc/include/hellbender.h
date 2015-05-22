#ifndef _HELLBENDER_H
#define _HELLBENDER_H

#include <sys/types.h>
#include <stdint.h>

#ifndef BREAK
extern unsigned long __force_order;
#define BREAK { asm volatile("xchg %%bx, %%bx" : "=m"(__force_order)); }
#endif

#ifndef BARRIER
#define BARRIER asm volatile("": : :"memory");
#endif

void syscall_print(const char* str);
void* syscall_set_program_break(void* set_or_null, intptr_t delta_or_zero);
void syscall_make_readonly(void* address, unsigned size);
void syscall_make_executable(void* address, unsigned size);

__attribute__((__noreturn__)) void syscall_exit(int status);
__attribute__((__noreturn__)) void syscall_iret(void);

unsigned syscall_current_exec_path(char* path);
void syscall_get_environment(int *argc, int *envc, char** strings);

void syscall_yield();
int syscall_spawn(const char *path, char *const argv[], char *const envp[]);

typedef struct sem sem_t;
int syscall_sem_open(sem_t *s, const char* name, int oflag);
int syscall_sem_create(sem_t *s, const char* name, int oflag,
                       mode_t mode, unsigned value);
int syscall_sem_post(sem_t *s);
int syscall_sem_wait(sem_t *s);

#endif
