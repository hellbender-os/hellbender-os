#ifndef __HELLBENDER_KERNEL_HIMEM_H__
#define __HELLBENDER_KERNEL_HIMEM_H__

#include "kernel.h"

void himem_init(struct MultibootData* data);

uintptr_t himem_alloc_page();
void himem_free_page(uintptr_t page);

#endif
