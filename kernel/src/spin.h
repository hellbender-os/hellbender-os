#ifndef __HELLBENDER_KERNEL_SPIN_H__
#define __HELLBENDER_KERNEL_SPIN_H__

#include "config.h"

#include <stdint.h>

typedef uint32_t __attribute__ ((aligned(CACHE_LINE))) spinlock_raw_t;

typedef struct spinlock {
  union {
    spinlock_raw_t lock;
    uint8_t dummy[CACHE_LINE];
  } __attribute__ ((packed));
} spinlock_t;

#define SPINLOCK_INIT {{0}}

#define SPIN_CONCAT(a,b) a ## b
#define SPIN_TOKEN(a,b) SPIN_CONCAT(a, b)

#define SPIN_GUARD(sp) spinlock_raw_t* SPIN_TOKEN(guard, __COUNTER__) __attribute__ ((__cleanup__(spin_guard_unlock))) = spin_guard_lock(&sp.lock);

#define SPIN_GUARD_RAW(lock) spinlock_raw_t* SPIN_TOKEN(guard, __COUNTER__) __attribute__ ((__cleanup__(spin_guard_unlock))) = spin_guard_lock(&lock);

// if returns 1 => lock was acquired and interrupts were disabled.
// else => no lock, no change in interrupts.
INLINE int spin_try_lock_raw(spinlock_raw_t* lock) {
  register int locked;
  asm volatile (
      "    mov $1, %1      \n"
      "    # try acquire   \n"
      "    lock bts $0, %0 \n"
      "    jnc 2f          \n"
      "    # lock busy     \n"
      "    mov $0, %1      \n"
      "2:                  \n"
      : "+m"(*lock), "=r"(locked) : : "cc"
      );
  BARRIER;
  return locked;
}

INLINE void spin_lock_raw(spinlock_raw_t* lock) {
  asm volatile (
      "    jmp 2f          \n"
      "1: # wait lock      \n"
      "    pause           \n"
      "    testl $1, %0    \n"
      "    jnz 1b          \n"
      "2: # try acquire    \n"
      "    lock bts $0, %0 \n"
      "    jc 1b           \n"
      : "+m"(*lock) : : "cc"
      );
  BARRIER;
}

INLINE int spin_try_lock(spinlock_t* s) {
  return spin_try_lock_raw(&s->lock);
}

INLINE void spin_lock(spinlock_t* s) {
  spin_lock_raw(&s->lock);
}

INLINE void spin_unlock_raw(spinlock_raw_t* lock) {
  BARRIER;
  asm volatile (
      "    movl $0, %0  \n"
      : "=m"(*lock)
      );
}

INLINE void spin_unlock(spinlock_t* s) {
  spin_unlock_raw(&s->lock);
}

INLINE spinlock_raw_t* spin_guard_lock(spinlock_raw_t* lock) {
  spin_lock_raw(lock);
  return lock;
}

INLINE void spin_guard_unlock(spinlock_raw_t** lock) {
  spin_unlock_raw(*lock);
}

#endif
