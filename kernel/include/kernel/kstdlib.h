#ifndef _KERNEL_KSTDLIB_H
#define _KERNEL_KSTDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__)) void kabort(void);
__attribute__((__noreturn__)) void khalt(void);
void* kmalloc(size_t size);
  
#ifdef __cplusplus
}
#endif

#endif
