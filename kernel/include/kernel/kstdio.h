#ifndef _KERNEL_KSTDIO_H
#define _KERNEL_KSTDIO_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

int kprintf(const char* __restrict, ...);

#ifdef __cplusplus
}
#endif

#endif
