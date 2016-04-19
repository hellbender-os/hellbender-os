#ifndef __HELLBENDER_KERNEL_HIMEM_H__
#define __HELLBENDER_KERNEL_HIMEM_H__

#include <stdint.h>

void himem_init();

uintptr_t himem_alloc_page();
void himem_free_page(uintptr_t page);

#endif
