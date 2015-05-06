#include <kernel/tss.h>
#include <kernel/kernel.h>

tss_entry_t kernel_tss;

void tss_update() {
  asm ("ltr %%ax"
       : /* no output registers */
       : "a"(SEL_TSS|3)
       :
       );
}
