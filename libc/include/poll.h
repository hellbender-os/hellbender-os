#ifndef _POLL_H
#define _POLL_H

// The Open Group Base Specifications Issue 7

struct pollfd {
  int    fd; //       The following descriptor being polled. 
  short  events; //   The input event flags (see below). 
  short  revents; //  The output event flags (see below). 
};

typedef unsigned nfds_t;
//An unsigned integer type used for the number of file descriptors.

#define POLLIN 01
//Data other than high-priority data may be read without blocking.

#define POLLRDNORM 02
// Normal data may be read without blocking.

#define POLLRDBAND 04
// Priority data may be read without blocking.

#define POLLPRI 010
// High priority data may be read without blocking.

#define POLLOUT 020
// Normal data may be written without blocking.

#define POLLWRNORM 040
// Equivalent to POLLOUT.

#define POLLWRBAND 0100
// Priority data may be written.

#define POLLERR 0200
// An error has occurred (revents only).

#define POLLHUP 0400
// Device has been disconnected (revents only).

#define POLLNVAL 01000
// Invalid fd member (revents only).

int   poll(struct pollfd [], nfds_t, int);

#endif
