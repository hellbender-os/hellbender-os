#include "kernel.h"
#include "vga.h"
#include "isr.h"
#include "signal.h"
#include "service.h"
#include "log.h"
#include "scheduler.h"
#include "rtc.h"

#include <stdint.h>
#include <sys/time.h>

/* Syscalls return data to user mode by directly modifying the return
 * state usign CPU_THREAD_STATE macro, e.g.
 *  CPU_THREAD_STATE->registers.rax = 1;
 * will return 1 to user mode.
 */

/* Syscalls can wait using 'continuations' with the following technique:
 *
 * Return value of syscall functions is either 0 to enter the user mode,
 * or a 'continuation function' to be called before entering user mode.
 *
 * Syscall can put thread to sleep and return a continuation function.
 * Scheduling will take place, and other thread will execute. When the
 * original thread is woken up, the continuation function is called.
 * User mode return values can then be set up.
 *
 * Continuations are executed as long as non-zero value is returned by
 * the continuation. This way the thread can sleep again until the required
 * data is available.
 */

uint64_t syscall_exit(int retval) {
  (void)retval;
  // TODO: actual exit
  log_error("syscall", "exit", "TODO");
}

uint64_t syscall_set_signal_mask(uint64_t blocked_mask) {
  signal_set_mask(cpu.current_thread, blocked_mask);
  return 0;
}

uint64_t syscall_set_irq_signal(unsigned irqnum, int signum) {
  isr_set_signal(irqnum, cpu.current_process, signum);
  return 0;
}

uint64_t continue_wait_signal() {
  // return signal value to usermode.
  CPU_THREAD_STATE->registers.rax = cpu.current_thread->signal.wait_signum;
  return 0;
}

uint64_t syscall_wait_signal(uint64_t wait_mask) {
  if (signal_wait(cpu.current_thread, wait_mask)) return continue_wait_signal();
  else return (uint64_t)continue_wait_signal;
}

uint64_t syscall_register_service(void* func) {
  CPU_THREAD_STATE->registers.rax = service_register((uint64_t)func);
  return 0;
}

uint64_t syscall_alloc_tables(uint64_t n_tables) {
  struct process_vmem* vmem =  process_alloc_vmem(n_tables * TABLE_SIZE);
  if (vmem) {
    CPU_THREAD_STATE->registers.rax = (uint64_t)vmem->base;
  } else {
    CPU_THREAD_STATE->registers.rax = 0;
  }
  return 0;
}

uint64_t syscall_log(const char *component, const char *function, const char *message) {
  log_info(component, function, message);
  return 0;
}

uint64_t syscall_create_thread(void (*func)(void *data), void *data, int priority) {
  struct process_vmem* vmem = process_alloc_vmem(USER_STACK_SIZE);
  if (vmem) {
    uintptr_t stack_top = (uintptr_t)vmem->base + vmem->size;
    struct thread *thread = thread_create(cpu.current_process, stack_top,
                                          (uintptr_t)func, (uintptr_t)data);
    if (!thread) log_error("syscall", "create_thread", "out of memory");
    thread->scheduler.priority = priority;
    list_insert(&cpu.current_process->threads, &thread->process_threads);
    scheduler_wakeup(thread);
    CPU_THREAD_STATE->registers.rax = (uint64_t)thread;
  } else {
    CPU_THREAD_STATE->registers.rax = 0;
  }
  return 0;
}

uint64_t syscall_timeofday(struct timeval *tp) {
  if (tp) {
    uint64_t nsec = rtc_nsec_since_boot();
    uint64_t secs = rtc_seconds_at_boot();
    tp->tv_sec = secs + nsec / 1000000000;
    tp->tv_usec = (nsec % 1000000000) / 1000;
    CPU_THREAD_STATE->registers.rax = nsec;
  } else {
    CPU_THREAD_STATE->registers.rax = 0;
  }
  return 0;
}

// this should be last.
uint64_t syscall_unknown() {
  // TODO: kill the process?
  log_error("syscall", "unknown", "TODO");
}
