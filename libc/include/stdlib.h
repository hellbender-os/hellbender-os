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

void* calloc(size_t num, size_t size);
void free(void* ptr);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);

void _srand(); // uses entropy from kernel to init rand
void srand(unsigned int seed);
int rand(void);

void exit(int status);
  
#ifdef __cplusplus
}
#endif

#endif
