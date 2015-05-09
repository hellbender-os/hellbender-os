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
#include <kernel/scheduler.h>

typedef struct pic_isr {
  thread_t* timer_thread;
  thread_t* thread;
  uint32_t pending; // boolean flags telling which interrupts are pending.
  uint32_t count[16]; // how many of said interrupts are pending.

  void* routines[16];
} pic_isr_t;

pic_isr_t pic_isr;

void pic_isr_initialize(thread_t *timer_thread, thread_t *other_thread,
                        core_service_t *core) {
  memset(&pic_isr, 0, sizeof(pic_isr));
  pic_isr.timer_thread = timer_thread;
  pic_isr.thread = other_thread;
  pic_isr.routines[0] = (void*)core->timer_isr;
  pic_isr.routines[1] = (void*)core->keyboard_isr;
}

__attribute__((__noreturn__))
void pic_isr_process_next() {
  while (pic_isr.pending && pic_isr.thread->state == THREAD_STATE_DEAD) {
    // find the pending interrupt with highest priority.
    unsigned pic_idx = 0;
    for (unsigned i = 0; i < 16; ++i) {
      if (pic_isr.pending & (1<<i)) {
        pic_idx = i;
        if (--pic_isr.count[i] == 0) {
          pic_isr.pending &= ~(1<<i);
        }
        break;
      }
    }
    pic_isr.thread->pic_line = pic_idx + 1;
    pic_isr.thread->start_address = pic_isr.routines[pic_idx];
    if (pic_isr.thread->start_address) {
      pic_isr.thread->state = THREAD_STATE_NEW;
    }
  }

  scheduler_goto_next();
}

void pic_isr_process(unsigned int_num) {
  // interrupt numbers are 32..39 and 112..119
  kprintf("interrupt %u\n", int_num);
  int_num -= 32;
  if (int_num > 8) int_num -= 96;

  // now int_num is from 0..15
  pic_isr.pending |= (1<<int_num);
  ++pic_isr.count[int_num];
  pic_isr_process_next();
}

void isr_routine_32() {
  //kprintf("isr_routine_32\n");
  pic_isr.timer_thread->pic_line = 1;
  pic_isr.timer_thread->start_address = pic_isr.routines[0];
  pic_isr.timer_thread->state = THREAD_STATE_NEW;
  thread_set_current(pic_isr.timer_thread);
  kernel_to_usermode();
}

void isr_routine_33() {
  //kprintf("isr_routine_33\n");
  pic_isr_process(33);
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
