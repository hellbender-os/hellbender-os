#ifndef _SCHED_H
#define _SCHED_H
// NONCONFORMING 2016

//Inclusion of the <sched.h> header may make visible all symbols from the <time.h> header.
//The <sched.h> header shall define the timespec structure as described in <time.h>.
#include <time.h>

//[PS] [Option Start] The <sched.h> header shall define the pid_t type as described in <sys/types.h>. [Option End]

//[SS|TSP] [Option Start] The <sched.h> header shall define the time_t type as described in <sys/types.h>. [Option End]

#include <sys/types.h> //NONCONFORMING

//The <sched.h> header shall define the sched_param structure, which shall include the scheduling parameters required for implementation of each supported scheduling policy. This structure shall include at least the following member:
struct sched_param {
  int sched_priority; // Process or thread execution scheduling priority. 
  //[SS|TSP] [Option Start] The sched_param structure defined in <sched.h> shall include the following members in addition to those specified above:
  int sched_ss_low_priority; // Low scheduling priority for sporadic server. 
  struct timespec sched_ss_repl_period; // Replenishment period for sporadic server. 
  struct timespec sched_ss_init_budget; // Initial budget for sporadic server. 
  int sched_ss_max_repl; // Maximum pending replenishments for sporadic server. 
  //[Option End]
};

//Each process or thread is controlled by an associated scheduling policy and priority. Associated with each policy is a priority range. Each policy definition specifies the minimum priority range for that policy. The priority ranges for each policy may overlap the priority ranges of other policies.

//Four scheduling policies are defined; others may be defined by the implementation. The four standard policies are indicated by the values of the following symbolic constants:

#define SCHED_FIFO     1
//[PS|TPS] [Option Start] First in-first out (FIFO) scheduling policy. [Option End]
#define SCHED_RR       2
//[PS|TPS] [Option Start] Round robin scheduling policy. [Option End]
#define SCHED_SPORADIC 3
//[SS|TSP] [Option Start] Sporadic server scheduling policy. [Option End]
#define SCHED_OTHER    4
//[PS|TPS] [Option Start] Another scheduling policy. [Option End]
//The values of these constants are distinct.

//The following shall be declared as functions and may also be defined as macros. Function prototypes shall be provided.

//[PS|TPS][Option Start]
int    sched_get_priority_max(int);
int    sched_get_priority_min(int);
//[Option End]
//[PS][Option Start]
int    sched_getparam(pid_t, struct sched_param *);
int    sched_getscheduler(pid_t);
//[Option End]
//[PS|TPS][Option Start]
int    sched_rr_get_interval(pid_t, struct timespec *);
//[Option End]
//[PS][Option Start]
int    sched_setparam(pid_t, const struct sched_param *);
int    sched_setscheduler(pid_t, int, const struct sched_param *);
//[Option End]
int    sched_yield(void);

#endif
