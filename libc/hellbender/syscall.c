#include <kernel/isr.h>
#include <kernel/syscall.h>
#include <hellbender.h>

void syscall_print(const char* str) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_PRINT), "d"(str));
}

void syscall_allocate(void* address, unsigned size) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_ALLOC), "d"(address), "c"(size));
}

__attribute__((__noreturn__)) void syscall_exit(int status) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_EXIT), "d"(status));
  __builtin_unreachable();
}

__attribute__((__noreturn__)) void syscall_iret(void) {
  asm(SYSIRET : "=m"(__force_order));
  __builtin_unreachable();
}

unsigned syscall_current_domain(domain_t *domain) {
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_CURRENT_DOMAIN), "d"(domain));
  return retval;
}

void syscall_yield(void) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_YIELD));
}

int syscall_sem_open(sem_t *s, const char* name, int oflag) {
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_SEM_OPEN), "d"(s), "b"(name), "c"(oflag));
  return retval;
}

int syscall_sem_post(sem_t *s) {
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_SEM_POST), "d"(s));
  return retval;
}

int syscall_sem_wait(sem_t *s) {
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_SEM_WAIT), "d"(s));
  return retval;
}

