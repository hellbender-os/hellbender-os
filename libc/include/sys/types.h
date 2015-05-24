#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <sys/types.h> // nonconforming: for pthread_t, size_t, uid_t

// The Open Group Base Specifications Issue 7

typedef unsigned blkcnt_t;
// Used for file block counts.

typedef unsigned blksize_t;
// Used for block sizes.

typedef unsigned clock_t;
// Used for system times in clock ticks or CLOCKS_PER_SEC; see <time.h>.

typedef unsigned clockid_t;
// Used for clock ID type in the clock and timer functions.

typedef unsigned dev_t;
// Used for device IDs.

typedef unsigned fsblkcnt_t;
// Used for file system block counts.

typedef unsigned fsfilcnt_t;
// Used for file system file counts.

typedef unsigned gid_t;
// Used for group IDs.

typedef unsigned id_t;
// Used as a general identifier; can be used to contain at least a pid_t, uid_t, or gid_t.

typedef unsigned ino_t;
// Used for file serial numbers.

typedef unsigned key_t;
// Used for XSI interprocess communication.

typedef unsigned mode_t;
// Used for some file attributes.

typedef unsigned nlink_t;
// Used for link counts.

typedef int off_t;
// Used for file sizes.

typedef unsigned pid_t;
// Used for process IDs and process group IDs.

typedef struct {
} pthread_attr_t;
// Used to identify a thread attribute object.

typedef struct {
} pthread_barrier_t;
// Used to identify a barrier.

typedef struct {
} pthread_barrierattr_t;
// Used to define a barrier attributes object.

typedef struct pthread_cond {
} pthread_cond_t;
// Used for condition variables.

typedef struct pthread_condattr {
} pthread_condattr_t;
// Used to identify a condition attribute object.

typedef struct {
} pthread_key_t;
// Used for thread-specific data keys.

typedef struct pthread_mutex {
} pthread_mutex_t;
// Used for mutexes.

typedef struct pthread_mutexattr {
} pthread_mutexattr_t;
// Used to identify a mutex attribute object.

typedef struct {
} pthread_once_t;
// Used for dynamic package initialization.

typedef struct {
} pthread_rwlock_t;
// Used for read-write locks.

typedef struct {
} pthread_rwlockattr_t;
// Used for read-write lock attributes.

typedef struct {
} pthread_spinlock_t;
// Used to identify a spin lock.

typedef struct {
} pthread_t;
// Used to identify a thread.

typedef __SIZE_TYPE__ size_t;
// Used for sizes of objects.

typedef int ssize_t;
// Used for a count of bytes or an error indication.

typedef int suseconds_t;
//  Used for time in microseconds.

typedef unsigned time_t;
// Used for time in seconds.

typedef int timer_t;
// Used for timer ID returned by timer_create().

typedef unsigned uid_t;
// Used for user IDs.

/*
All of the types shall be defined as arithmetic types of an appropriate length, with the following exceptions:

pthread_attr_t
pthread_barrier_t
pthread_barrierattr_t
pthread_cond_t
pthread_condattr_t
pthread_key_t
pthread_mutex_t
pthread_mutexattr_t
pthread_once_t
pthread_rwlock_t
pthread_rwlockattr_t
pthread_spinlock_t
pthread_t

mode_t shall be an integer type.
dev_t shall be an integer type.
nlink_t, uid_t, gid_t, and id_t shall be integer types.
blkcnt_t and off_t shall be signed integer types.
fsblkcnt_t, fsfilcnt_t, and ino_t shall be defined as unsigned integer types.
size_t shall be an unsigned integer type.
blksize_t, pid_t, and ssize_t shall be signed integer types.
clock_t shall be an integer or real-floating type.
time_t shall be an integer type.
The type ssize_t shall be capable of storing values at least in the range [-1, {SSIZE_MAX}].
 The type suseconds_t shall be a signed integer type capable of storing values at least in the range [-1, 1000000].

The implementation shall support one or more programming environments in which the widths of blksize_t, pid_t, size_t, ssize_t, and suseconds_t are no greater than the width of type long.
*/


#endif
