#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/isr.h>
#include <kernel/pic.h>
#include <kernel/thread.h>
#include <kernel/module.h>
#include <kernel/process.h>
#include <kernel/scheduler.h>

typedef struct pic_isr {
  volatile uint64_t ticks;
  uint64_t tick_time;  // now long one tick takes, in RDTSC units.
  uint64_t tick_start; // used to estimate measure tick_time.
  
  thread_t* timer_thread;
  thread_t* thread;
  uint32_t pending; // boolean flags telling which interrupts are pending.
  uint32_t count[16]; // how many of said interrupts are pending.

  void* routines[16];
} pic_isr_t;

pic_isr_t pic_isr;

void pic_isr_stage_4_init(core_service_t *core, process_t *core_process) {
  memset(&pic_isr, 0, sizeof(pic_isr));
  
  // we need 2 threads: one to process timer events (ensuring proper scheduing),
  // and the other one for processing all other interrupts.
  thread_t *timer_thread = thread_create(core_process->domain, NULL);
  thread_t *other_thread = thread_create(core_process->domain, NULL);
  scheduler_set_realtime_thread(timer_thread);
  scheduler_add_thread(other_thread);

  memset(&pic_isr, 0, sizeof(pic_isr));
  pic_isr.timer_thread = timer_thread;
  pic_isr.thread = other_thread;

  pic_isr.routines[0] = (void*)core->timer_isr;
  pic_isr.routines[1] = (void*)core->ps2_chan1_isr;
  //pic_isr.routines[12] = (void*)core->ps2_chan2_isr;

  pic_enable(32); // timer0
  pic_enable(33); // ps2 channel 1
  //pic_enable(116); // ps2 channel 2
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
  //printf("interrupt %u\n", int_num);
  int_num -= 32;
  if (int_num > 8) int_num -= 96;

  // now int_num is from 0..15
  pic_isr.pending |= (1<<int_num);
  ++pic_isr.count[int_num];
  pic_isr_process_next();
}

void isr_routine_32() {
  //printf("isr_routine_32\n");

  // estimate the RDTSC time between ticks.
  if (pic_isr.ticks < 100) {
    if (pic_isr.ticks == 0) {
      pic_isr.tick_start = rdtsc();
    } else if (pic_isr.ticks < 5) {
      // we want the first estimate of tick_time as soon as possible,
      // but we will skip the first few interrupts for the final estimate
      // because emulator speeds seem to fluctuate after start.
      uint64_t tick_end = rdtsc();
      pic_isr.tick_time = tick_end - pic_isr.tick_start;
      pic_isr.tick_start = rdtsc();
    } else {
      uint64_t tick_end = rdtsc();
      pic_isr.tick_time =
        (tick_end - pic_isr.tick_start) / (pic_isr.ticks - 4);
    }
  }
  ++pic_isr.ticks;
  
  pic_isr.timer_thread->pic_line = 1;
  pic_isr.timer_thread->start_address = pic_isr.routines[0];
  pic_isr.timer_thread->state = THREAD_STATE_NEW;
  thread_set_current(pic_isr.timer_thread);
  kernel_to_usermode();
}

void isr_routine_33() {
  //printf("isr_routine_33\n");
  pic_isr_process(33);
}

void isr_routine_34() {
  printf("isr_routine_34\n");
}

void isr_routine_35() {
  printf("isr_routine_35\n");
}

void isr_routine_36() {
  printf("isr_routine_36\n");
}

void isr_routine_37() {
  printf("isr_routine_37\n");
}

void isr_routine_38() {
  printf("isr_routine_38\n");
}

void isr_routine_39() {
  //if (!isr_pic_check_39()) return;
  printf("isr_routine_39\n");
}

void isr_routine_112() {
  printf("isr_routine_112\n");
}

void isr_routine_113() {
  printf("isr_routine_113\n");
}

void isr_routine_114() {
  printf("isr_routine_114\n");
}

void isr_routine_115() {
  printf("isr_routine_115\n");
}

void isr_routine_116() {
  printf("isr_routine_116\n");
}

void isr_routine_117() {
  printf("isr_routine_117\n");
}

void isr_routine_118() {
  printf("isr_routine_118\n");
}

void isr_routine_119() {
  printf("isr_routine_119\n");
}
