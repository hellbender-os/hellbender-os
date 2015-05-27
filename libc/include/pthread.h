#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <sys/types.h> // nonconforming: lots of pthread_ types.

// The Open Group Base Specifications Issue 7

#include <sched.h>
#include <time.h>

/*
The <pthread.h> header shall define the following symbolic constants:

PTHREAD_BARRIER_SERIAL_THREAD
PTHREAD_CANCEL_ASYNCHRONOUS
PTHREAD_CANCEL_ENABLE
PTHREAD_CANCEL_DEFERRED
PTHREAD_CANCEL_DISABLE
PTHREAD_CANCELED
PTHREAD_CREATE_DETACHED
PTHREAD_CREATE_JOINABLE
[TPS] [Option Start]
PTHREAD_EXPLICIT_SCHED
PTHREAD_INHERIT_SCHED
[Option End]
PTHREAD_MUTEX_DEFAULT
PTHREAD_MUTEX_ERRORCHECK
PTHREAD_MUTEX_NORMAL
PTHREAD_MUTEX_RECURSIVE
PTHREAD_MUTEX_ROBUST
PTHREAD_MUTEX_STALLED
PTHREAD_ONCE_INIT
[RPI|TPI] [Option Start]
PTHREAD_PRIO_INHERIT
[Option End]
[MC1] [Option Start]
PTHREAD_PRIO_NONE
[Option End]
[RPP|TPP] [Option Start]
PTHREAD_PRIO_PROTECT
[Option End]
PTHREAD_PROCESS_SHARED
PTHREAD_PROCESS_PRIVATE
[TPS] [Option Start]
PTHREAD_SCOPE_PROCESS
PTHREAD_SCOPE_SYSTEM
[Option End]

The <pthread.h> header shall define the following compile-time constant expressions valid as initializers for the following types:

Name

Initializer for Type

*/
#define PTHREAD_COND_INITIALIZER (pthread_cond_t){ .waiting = 0, .released = 0, .entry_nr = 0, .exit_nr = 0 }

#define PTHREAD_MUTEX_INITIALIZER (pthread_mutex_t){ .count = 1, .entry_nr = 0, .exit_nr = 0, .owner = 0 }
/*
PTHREAD_RWLOCK_INITIALIZER

pthread_rwlock_t

The <pthread.h> header shall define the pthread_attr_t, pthread_barrier_t, pthread_barrierattr_t, pthread_cond_t, pthread_condattr_t, pthread_key_t, pthread_mutex_t, pthread_mutexattr_t, pthread_once_t, pthread_rwlock_t, pthread_rwlockattr_t, pthread_spinlock_t, and pthread_t types as described in <sys/types.h>.
*/

int   pthread_atfork(void (*)(void), void (*)(void), void(*)(void));
int   pthread_attr_destroy(pthread_attr_t *);
int   pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int   pthread_attr_getguardsize(const pthread_attr_t *,
          size_t *);
int   pthread_attr_getinheritsched(const pthread_attr_t *,
          int *);
int   pthread_attr_getschedparam(const pthread_attr_t *,
          struct sched_param *);
int   pthread_attr_getschedpolicy(const pthread_attr_t *,
          int *);
int   pthread_attr_getscope(const pthread_attr_t *,
          int *);
int   pthread_attr_getstack(const pthread_attr_t *,
          void **, size_t *);
int   pthread_attr_getstacksize(const pthread_attr_t *,
          size_t *);
int   pthread_attr_init(pthread_attr_t *);
int   pthread_attr_setdetachstate(pthread_attr_t *, int);
int   pthread_attr_setguardsize(pthread_attr_t *, size_t);
int   pthread_attr_setinheritsched(pthread_attr_t *, int);
int   pthread_attr_setschedparam(pthread_attr_t *,
          const struct sched_param *);
int   pthread_attr_setschedpolicy(pthread_attr_t *, int);
int   pthread_attr_setscope(pthread_attr_t *, int);
int   pthread_attr_setstack(pthread_attr_t *, void *, size_t);
int   pthread_attr_setstacksize(pthread_attr_t *, size_t);
int   pthread_barrier_destroy(pthread_barrier_t *);
int   pthread_barrier_init(pthread_barrier_t *,
          const pthread_barrierattr_t *, unsigned);
int   pthread_barrier_wait(pthread_barrier_t *);
int   pthread_barrierattr_destroy(pthread_barrierattr_t *);
int   pthread_barrierattr_getpshared(
          const pthread_barrierattr_t *, int *);
int   pthread_barrierattr_init(pthread_barrierattr_t *);
int   pthread_barrierattr_setpshared(pthread_barrierattr_t *, int);
int   pthread_cancel(pthread_t);
void  pthread_cleanup_pop(int);
void  pthread_cleanup_push(void (*)(void*), void *);
int   pthread_cond_broadcast(pthread_cond_t *);
int   pthread_cond_destroy(pthread_cond_t *);
int   pthread_cond_init(pthread_cond_t *,
          const pthread_condattr_t *);
int   pthread_cond_signal(pthread_cond_t *);
int   pthread_cond_timedwait(pthread_cond_t *,
          pthread_mutex_t *, const struct timespec *);
int   pthread_cond_wait(pthread_cond_t *,
          pthread_mutex_t *);
int   pthread_condattr_destroy(pthread_condattr_t *);
int   pthread_condattr_getclock(const pthread_condattr_t *,
          clockid_t *);
int   pthread_condattr_getpshared(const pthread_condattr_t *,
          int *);
int   pthread_condattr_init(pthread_condattr_t *);
int   pthread_condattr_setclock(pthread_condattr_t *, clockid_t);
int   pthread_condattr_setpshared(pthread_condattr_t *, int);
int   pthread_create(pthread_t *, const pthread_attr_t *,
          void *(*)(void*), void *);
int   pthread_detach(pthread_t);
int   pthread_equal(pthread_t, pthread_t);
void  pthread_exit(void *);
int   pthread_getconcurrency(void);
int   pthread_getcpuclockid(pthread_t, clockid_t *);
int   pthread_getschedparam(pthread_t, int *,
          struct sched_param *);
void *pthread_getspecific(pthread_key_t);
int   pthread_join(pthread_t, void **);
int   pthread_key_create(pthread_key_t *, void (*)(void*));
int   pthread_key_delete(pthread_key_t);
int   pthread_mutex_consistent(pthread_mutex_t *);
int   pthread_mutex_destroy(pthread_mutex_t *);
int   pthread_mutex_getprioceiling(const pthread_mutex_t *,
          int *);
int   pthread_mutex_init(pthread_mutex_t *,
          const pthread_mutexattr_t *);
int   pthread_mutex_lock(pthread_mutex_t *);
int   pthread_mutex_setprioceiling(pthread_mutex_t *, int,
          int *);
int   pthread_mutex_timedlock(pthread_mutex_t *,
          const struct timespec *);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);
int   pthread_mutexattr_destroy(pthread_mutexattr_t *);
int   pthread_mutexattr_getprioceiling(
          const pthread_mutexattr_t *, int *);
int   pthread_mutexattr_getprotocol(const pthread_mutexattr_t *,
          int *);
int   pthread_mutexattr_getpshared(const pthread_mutexattr_t *,
          int *);
int   pthread_mutexattr_getrobust(const pthread_mutexattr_t *,
          int *);
int   pthread_mutexattr_gettype(const pthread_mutexattr_t *,
          int *);
int   pthread_mutexattr_init(pthread_mutexattr_t *);
int   pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
int   pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);
int   pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
int   pthread_mutexattr_setrobust(pthread_mutexattr_t *, int);
int   pthread_mutexattr_settype(pthread_mutexattr_t *, int);
int   pthread_once(pthread_once_t *, void (*)(void));
int   pthread_rwlock_destroy(pthread_rwlock_t *);
int   pthread_rwlock_init(pthread_rwlock_t *,
          const pthread_rwlockattr_t *);
int   pthread_rwlock_rdlock(pthread_rwlock_t *);
int   pthread_rwlock_timedrdlock(pthread_rwlock_t *,
          const struct timespec *);
int   pthread_rwlock_timedwrlock(pthread_rwlock_t *,
          const struct timespec *);
int   pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int   pthread_rwlock_trywrlock(pthread_rwlock_t *);
int   pthread_rwlock_unlock(pthread_rwlock_t *);
int   pthread_rwlock_wrlock(pthread_rwlock_t *);
int   pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
int   pthread_rwlockattr_getpshared(
          const pthread_rwlockattr_t *, int *);
int   pthread_rwlockattr_init(pthread_rwlockattr_t *);
int   pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);
pthread_t
      pthread_self(void);
int   pthread_setcancelstate(int, int *);
int   pthread_setcanceltype(int, int *);
int   pthread_setconcurrency(int);
int   pthread_setschedparam(pthread_t, int,
          const struct sched_param *);
int   pthread_setschedprio(pthread_t, int);
int   pthread_setspecific(pthread_key_t, const void *);
int   pthread_spin_destroy(pthread_spinlock_t *);
int   pthread_spin_init(pthread_spinlock_t *, int);
int   pthread_spin_lock(pthread_spinlock_t *);
int   pthread_spin_trylock(pthread_spinlock_t *);
int   pthread_spin_unlock(pthread_spinlock_t *);
void  pthread_testcancel(void);

#endif
