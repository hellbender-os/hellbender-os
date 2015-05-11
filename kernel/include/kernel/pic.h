#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

void pic_stage_3_init();

void pic_eoi(unsigned interrupt);
int pic_check_39();
void pic_enable(unsigned interrupt);
void pic_disable(unsigned interrupt);

#endif
