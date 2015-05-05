#ifndef _KERNEL_DOMAIN_H
#define _KERNEL_DOMAIN_H

#include <stddef.h>

#include <kernel/kernel.h>

typedef struct domain domain_t;

// _bottom for applications, _top for services, _existing for core.
domain_t* domain_create_bottom();
domain_t* domain_create_top();
domain_t* domain_create_existing(void* existing, void* start);

void* domain_grow_stack(domain_t *domain, size_t size); // returns new bottom.
void* domain_grow_heap(domain_t *domain, size_t size); // returns new top.

void domain_enter_ring3(domain_t *domain);

#endif
