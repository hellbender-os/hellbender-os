#ifndef _SCHED_H
#define _SCHED_H

// The Open Group Base Specifications Issue 7

#include <time.h>

// pid_t type as described in <sys/types.h>.
// time_t type as described in <sys/types.h>.
// struct timespec as described in <time.h>.

struct sched_param {
  int       sched_priority; //        Process or thread execution scheduling priority. 
};

//Each process or thread is controlled by an associated scheduling policy and priority. Associated with each policy is a priority range. Each policy definition specifies the minimum priority range for that policy. The priority ranges for each policy may overlap the priority ranges of other policies.

//Four scheduling policies are defined; others may be defined by the implementation. The four standard policies are indicated by the values of the following symbolic constants:

#define SCHED_FIFO 1
//[PS|TPS] [Option Start] First in-first out (FIFO) scheduling policy. [Option End]

#define SCHED_RR 2
//[PS|TPS] [Option Start] Round robin scheduling policy. [Option End]

#define SCHED_OTHER 3
//[PS|TPS] [Option Start] Another scheduling policy. [Option End]

int    sched_get_priority_max(int);
int    sched_get_priority_min(int);
int    sched_getparam(pid_t, struct sched_param *);
int    sched_getscheduler(pid_t);
int    sched_rr_get_interval(pid_t, struct timespec *);
int    sched_setparam(pid_t, const struct sched_param *);
int    sched_setscheduler(pid_t, int, const struct sched_param *);
int    sched_yield(void);


#endif
