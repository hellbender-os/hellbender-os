#ifndef __LIBC_HEAP_H__
#define __LIBC_HEAP_H__

#include <stddef.h>

void* _heap_alloc(size_t size);
void _heap_free(void *ptr);
void* _heap_realloc(void *ptr, size_t size);

#endif
