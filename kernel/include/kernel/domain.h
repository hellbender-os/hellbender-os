#ifndef _KERNEL_DOMAIN_H
#define _KERNEL_DOMAIN_H

#include <stddef.h>

#include <kernel/kernel.h>

typedef struct domain domain_t;

domain_t* domain_create();
void* domain_alloc_data(domain_t *domain, size_t size);
void* domain_alloc_code(domain_t *domain, size_t size);

#endif
