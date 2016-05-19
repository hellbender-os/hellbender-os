#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <sys/types.h> // nonconforming: time_t and suseconds_t

// The Open Group Base Specifications Issue 7

#include <signal.h>
#include <time.h>

struct timeval {
  time_t         tv_sec; //      Seconds. 
  suseconds_t    tv_usec; //     Microseconds. 
};

typedef struct {
} fd_set;

//#define FD_SETSIZE
// Maximum number of file descriptors in an fd_set structure.

void FD_CLR(int, fd_set *);
int  FD_ISSET(int, fd_set *);
void FD_SET(int, fd_set *);
void FD_ZERO(fd_set *);

int  pselect(int, fd_set *, fd_set *, fd_set *,
             const struct timespec *, const sigset_t *);
int  select(int, fd_set *, fd_set *, fd_set *,
            struct timeval *);

#endif
