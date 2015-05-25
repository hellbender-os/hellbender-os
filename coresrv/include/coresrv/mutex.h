#ifndef _KERNEL_MUTEX_H
#define _KERNEL_MUTEX_H

#include <kernel/syscall.h>

typedef struct posix_mutex posix_mutex_t;
__SYS__ int syscall_mutex_init(posix_mutex_t *mutex);
__SYS__ int syscall_mutex_lock(posix_mutex_t *mutex);

#endif
