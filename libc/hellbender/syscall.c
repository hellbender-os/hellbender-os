#include <kernel/isr.h>
#include <kernel/syscall.h>
#include <hellbender.h>

void syscall_print(const char* str) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_PRINT), "d"(str));
}

void* syscall_set_program_break(void* set_or_null, intptr_t delta_or_zero) {
  register void* retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_SET_PROGRAM_BREAK), "d"(set_or_null), "c"(delta_or_zero));
  return retval;
}

void syscall_make_readonly(void* address, unsigned size) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_MAKE_READONLY), "d"(address), "c"(size));
}

void syscall_make_executable(void* address, unsigned size) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_MAKE_EXECUTABLE), "d"(address), "c"(size));
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

unsigned syscall_current_exec_path(char *path) {
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_CURRENT_EXEC_PATH), "d"(path));
  return retval;
}

void syscall_get_environment(int *argc, int *envc, char** strings) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_GET_ENVIRONMENT), "d"(argc), "b"(envc), "c"(strings));
}

void syscall_yield(void) {
  asm(SYSCALL : "=m"(__force_order)
      : "a"(SYSCALL_YIELD));
}

int syscall_spawn(const char *path, char *const argv[], char *const envp[]) {
  register int retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_SPAWN), "d"(path), "b"(argv), "c"(envp));
  return retval;
}

int syscall_sem_open(sem_t *s, const char* name, int oflag) {
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_SEM_OPEN), "d"(s), "b"(name), "c"(oflag));
  return retval;
}

int syscall_sem_create(sem_t *s, const char* name, int oflag,
                       mode_t mode, unsigned value) {
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_SEM_CREATE), "d"(s), "b"(name), "c"(oflag),
        "D"(mode), "S"(value));
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

int syscall_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {
  return 0;/*
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_MUTEX_INIT), "d"(mutex), "b"(attr));
  return retval;
           */
}

int syscall_mutex_lock(pthread_mutex_t *mutex) {
  return 0;/*
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_MUTEX_LOCK), "d"(mutex));
  return retval;
           */
}

int syscall_mutex_unlock(pthread_mutex_t *mutex) {
  return 0;/*
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_MUTEX_UNLOCK), "d"(mutex));
  return retval;
           */
}

int syscall_cond_init(pthread_cond_t *cond,
                      const pthread_condattr_t *attr) {
  return 0;/*
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_COND_INIT), "d"(cond), "b"(attr));
  return retval;
           */
}

int syscall_cond_wait(pthread_cond_t *cond,
                      pthread_mutex_t *mutex) {
  return 0;/*
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_COND_WAIT), "d"(cond), "b"(mutex));
  return retval;
           */
}

int syscall_cond_broadcast(pthread_cond_t *cond) {
  return 0;/*
  register unsigned retval __asm__("eax");
  asm(SYSCALL : "=r"(retval), "=m"(__force_order)
      : "a"(SYSCALL_COND_BROADCAST), "d"(cond));
  return retval;
           */
}
