#ifndef __HELLBENDER_GUARD_H__
#define __HELLBENDER_GUARD_H__

#include "inline.h"

typedef struct pthread_mutex pthread_mutex_t;

// Various definitons for RAII lock guards.

#define _GUARD_CONCAT(a,b) a ## b
#define _GUARD_TOKEN(a,b) _GUARD_CONCAT(a, b)

// pthread_mutex

#define GUARD_PTHREAD_MUTEX(mutex) pthread_mutex_t* _GUARD_TOKEN(_guard, __COUNTER__) __attribute__ ((__cleanup__(_guard_pthread_mutex_unlock))) = _guard_pthread_mutex_lock(mutex);

INLINE pthread_mutex_t *_guard_pthread_mutex_lock(pthread_mutex_t *mutex) {
  pthread_mutex_lock(mutex);
  return mutex;
}

INLINE void _guard_pthread_mutex_unlock(pthread_mutex_t **mutex) {
  pthread_mutex_unlock(*mutex);
}

// pthread_rwlock

#define GUARD_PTHREAD_RWLOCK_READ(rwlock) pthread_rwlock_t* _GUARD_TOKEN(_guard, __COUNTER__) __attribute__ ((__cleanup__(_guard_pthread_rwlock_unlock))) = _guard_pthread_rwlock_rdlock(rwlock);

#define GUARD_PTHREAD_RWLOCK_WRITE(rwlock) pthread_rwlock_t* _GUARD_TOKEN(_guard, __COUNTER__) __attribute__ ((__cleanup__(_guard_pthread_rwlock_unlock))) = _guard_pthread_rwlock_wrlock(rwlock);

INLINE pthread_mutex_t *_guard_pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
  pthread_rwlock_rdlock(rwlock);
  return rwlock;
}

INLINE pthread_mutex_t *_guard_pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
  pthread_rwlock_wrlock(rwlock);
  return rwlock;
}

INLINE void _guard_pthread_rwlock_unlock(pthread_rwlock_t **rwlock) {
  pthread_rwlock_unlock(*rwlock);
}

#endif
