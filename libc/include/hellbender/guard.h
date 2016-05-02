#ifndef __HELLBENDER_GUARD_H__
#define __HELLBENDER_GUARD_H__

#include "inline.h"

// Various definitons for RAII lock guards.

#define _GUARD_CONCAT(a,b) a ## b
#define _GUARD_TOKEN(a,b) _GUARD_CONCAT(a, b)

#define GUARD_PTHREAD_MUTEX(mutex) pthread_mutex_t* _GUARD_TOKEN(_guard, __COUNTER__) __attribute__ ((__cleanup__(_guard_pthread_mutex_unlock))) = _guard_pthread_mutex_lock(mutex);

INLINE pthread_mutex_t *_guard_pthread_mutex_lock(pthread_mutex_t *mutex) {
  pthread_mutex_lock(mutex);
  return mutex;
}

INLINE void _guard_pthread_mutex_unlock(pthread_mutex_t **mutex) {
  pthread_mutex_unlock(*mutex);
}

#endif
