#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

//The clock_t type shall be defined as described in <sys/types.h> .
#include "types.h" // nonconforming

// The Open Group Base Specifications Issue 7

// The <sys/times.h> header shall define the structure tms, which is returned by times() and includes at least the following members:
struct tms {
  clock_t  tms_utime; //  User CPU time. 
  clock_t  tms_stime; //  System CPU time. 
  clock_t  tms_cutime; // User CPU time of terminated child processes. 
  clock_t  tms_cstime; // System CPU time of terminated child processes. 
};

//The following shall be declared as a function and may also be defined as a macro. A function prototype shall be provided.

clock_t times(struct tms *);

#endif
