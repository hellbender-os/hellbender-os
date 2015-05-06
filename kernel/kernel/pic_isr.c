#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/isr.h>
#include <kernel/io.h>
#include <kernel/thread.h>
#include <kernel/module.h>

void isr_routine_32() {
  kprintf("isr_routine_32\n");
  isr_pic_eoi(32);
}

void isr_routine_33() {
  kprintf("isr_routine_33\n");
  // Make thread33 current context.
  // TODO: if pic_processor still set, queue interrupt!
  
  CURRENT_THREAD->state = THREAD_STATE_NEW;
  CURRENT_THREAD->start_address = (void*)CORE_SERVICE->keyboard_isr;
  CURRENT_THREAD->pic_processor = 33;
  kernel_to_usermode();
  
  /*
  unsigned key = inb(0x60);
  kprintf("KEY=%u\n", key);
  */
}

void isr_routine_34() {
  kprintf("isr_routine_34\n");
}

void isr_routine_35() {
  kprintf("isr_routine_35\n");
}

void isr_routine_36() {
  kprintf("isr_routine_36\n");
}

void isr_routine_37() {
  kprintf("isr_routine_37\n");
}

void isr_routine_38() {
  kprintf("isr_routine_38\n");
}

void isr_routine_39() {
  if (!isr_pic_check_39()) return;
  kprintf("isr_routine_39\n");
}

void isr_routine_112() {
  kprintf("isr_routine_112\n");
}

void isr_routine_113() {
  kprintf("isr_routine_113\n");
}

void isr_routine_114() {
  kprintf("isr_routine_114\n");
}

void isr_routine_115() {
  kprintf("isr_routine_115\n");
}

void isr_routine_116() {
  kprintf("isr_routine_116\n");
}

void isr_routine_117() {
  kprintf("isr_routine_117\n");
}

void isr_routine_118() {
  kprintf("isr_routine_118\n");
}

void isr_routine_119() {
  kprintf("isr_routine_119\n");
}
