#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <sys/types.h> //  nonconforming: time_t and suseconds_t

// The Open Group Base Specifications Issue 7

#include <sys/select.h>

struct itimerval {
  struct timeval it_interval; // Timer interval. 
  struct timeval it_value; //    Current value. 
};
  
#define ITIMER_REAL 1
// Decrements in real time.

#define ITIMER_VIRTUAL 2
// Decrements in process virtual time.

#define ITIMER_PROF 3
// Decrements both in process virtual time and when the system is running on behalf of the process.


int   getitimer(int, struct itimerval *);
int   gettimeofday(struct timeval *, void *);
int   setitimer(int, const struct itimerval *,
                struct itimerval *);
int   select(int, fd_set *, fd_set *, fd_set *,
             struct timeval *);
int   utimes(const char *, const struct timeval [2]);

#endif
