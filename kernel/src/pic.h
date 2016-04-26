#ifndef __HELLBENDER_KERNEL_PIC_H__
#define __HELLBENDER_KERNEL_PIC_H__

#include "config.h"
#include "io.h"

#include <stdint.h>

/* Re-assigns PIC interrupts to 0x20->
 */
void pic_init();

#endif
