#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <sys/types.h> // nonconforming: id_t

// The Open Group Base Specifications Issue 7

#include <sys/time.h>

#define PRIO_PROCESS 1
// Identifies the who argument as a process ID.

#define PRIO_PGRP 2
// Identifies the who argument as a process group ID.

#define PRIO_USER 3
// Identifies the who argument as a user ID.

typedef unsigned rlim_t;
// Unsigned integer type used for limit values.

#define RLIM_INFINITY 4294967295u
// A value of rlim_t indicating no limit.

#define RLIM_SAVED_MAX 4294967295u
// A value of type rlim_t indicating an unrepresentable saved hard limit.

#define RLIM_SAVED_CUR 4294967295u
// A value of type rlim_t indicating an unrepresentable saved soft limit.

// On implementations where all resource limits are representable in an object of type rlim_t, RLIM_SAVED_MAX and RLIM_SAVED_CUR need not be distinct from RLIM_INFINITY.

#define RUSAGE_SELF 1
// Returns information about the current process.

#define RUSAGE_CHILDREN
// Returns information about children of the current process.

struct rlimit {
  rlim_t rlim_cur; //  The current (soft) limit. 
  rlim_t rlim_max; //  The hard limit. 
};

struct rusage {
  struct timeval ru_utime; //  User time used. 
  struct timeval ru_stime; //  System time used. 
};

#define RLIMIT_CORE 1
// Limit on size of core file.

#define RLIMIT_CPU 2
// Limit on CPU time per process.

#define RLIMIT_DATA 3
// Limit on data segment size.

#define RLIMIT_FSIZE 4
// Limit on file size.

#define RLIMIT_NOFILE 5
// Limit on number of open files.

#define RLIMIT_STACK 6
// Limit on stack size.

#define RLIMIT_AS 7
// Limit on address space size.

int  getpriority(int, id_t);
int  getrlimit(int, struct rlimit *);
int  getrusage(int, struct rusage *);
int  setpriority(int, id_t, int);
int  setrlimit(int, const struct rlimit *);

#endif
