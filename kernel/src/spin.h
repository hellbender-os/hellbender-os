#ifndef __HELLBENDER_KERNEL_SPIN_H__
#define __HELLBENDER_KERNEL_SPIN_H__

#include "config.h"

typedef uint32_t __attribute__ ((aligned(CACHE_LINE))) spinlock_raw_t;

typedef struct spinlock {
  union {
    spinlock_raw_t lock;
    uint8_t dummy[CACHE_LINE];
  } u __attribute__ ((packed));
} spinlock_t;

#define SPINLOCK_INIT {{0}}

#define SPIN_GUARD(guard, sp) spinlock_raw_t* guard __attribute__ ((__cleanup__(spin_guard_unlock))) = spin_guard_lock(&sp.u.lock);

#define SPIN_GUARD_RAW(guard, lock) spinlock_raw_t* guard __attribute__ ((__cleanup__(spin_guard_unlock))) = spin_guard_lock(&lock);

INLINE void spin_lock_raw(spinlock_raw_t* lock) {
  asm("    jmp 2f          \n"
      "1: # wait lock      \n"
      "    pause           \n"
      "    testl $1, %0    \n"
      "    jnz 1b          \n"
      "2: # try acquire    \n"
      "    lock bts $0, %0 \n"
      "    jc 1b           \n"
      : "+m"(*lock) : : "cc"
      );
}

INLINE void spin_lock(spinlock_t* s) {
  spin_lock_raw(&s->u.lock);
}

INLINE void spin_unlock_raw(spinlock_raw_t* lock) {
  asm("    movl $0, %0  \n"
      : "=m"(*lock)
      );
}

INLINE void spin_unlock(spinlock_t* s) {
  spin_unlock_raw(&s->u.lock);
}

INLINE spinlock_raw_t* spin_guard_lock(spinlock_raw_t* lock) {
  spin_lock_raw(lock);
  return lock;
}

INLINE void spin_guard_unlock(spinlock_raw_t** lock) {
  spin_unlock_raw(*lock);
}

#endif
