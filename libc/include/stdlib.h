#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

char* itoa(int value, char* str, int base);
char* _utoa(unsigned value, char* str, int base);

void* malloc(size_t size);
  
#ifdef __cplusplus
}
#endif

#endif
