#ifndef _TIME_H
#define _TIME_H

#include <limits.h> // nonconforming: for TZNAME_MAX
#include <sys/types.h> // nonconforming: for clock_t, size_t, time_t, clockid_t, timer_t, pid_t
#include <locale.h> // nonconforming: for locale_t

// The Open Group Base Specifications Issue 7

#include <signal.h>

// clock_t, size_t, time_t, types as described in <sys/types.h>.
// clockid_t and timer_t types as described in <sys/types.h>.
// locale_t type as described in <locale.h>.
// pid_t type as described in <sys/types.h>.

struct sigevent;

struct tm {
  int    tm_sec; //   Seconds [0,60]. 
  int    tm_min; //   Minutes [0,59]. 
  int    tm_hour; //  Hour [0,23]. 
  int    tm_mday; //  Day of month [1,31]. 
  int    tm_mon; //   Month of year [0,11]. 
  int    tm_year; //  Years since 1900. 
  int    tm_wday; //  Day of week [0,6] (Sunday =0). 
  int    tm_yday; //  Day of year [0,365]. 
  int    tm_isdst; // Daylight Savings flag. 
};

//The value of tm_isdst shall be positive if Daylight Savings Time is in effect, 0 if Daylight Savings Time is not in effect, and negative if the information is not available.

struct timespec {
  time_t  tv_sec; //    Seconds. 
  long    tv_nsec; //   Nanoseconds. 
};

struct itimerspec {
  struct timespec  it_interval; //  Timer period. 
  struct timespec  it_value; //     Timer expiration. 
};

//NULL As described in <stddef.h>.
#define CLOCKS_PER_SEC 1000000
//A number used to convert the value returned by the clock() function into seconds. The value shall be an expression with type clock_t. [XSI] [Option Start]  The value of CLOCKS_PER_SEC shall be 1 million on XSI-conformant systems. However, it may be variable on other systems, and it should not be assumed that CLOCKS_PER_SEC is a compile-time constant. [Option End]

/*
[CX] [Option Start] The <time.h> header shall define the following symbolic constants. The values shall have a type that is assignment-compatible with clockid_t. [Option End]

CLOCK_MONOTONIC
[MON] [Option Start]
The identifier for the system-wide monotonic clock, which is defined as a clock measuring real time, whose value cannot be set via clock_settime() and which cannot have negative clock jumps. The maximum possible clock jump shall be implementation-defined. [Option End]
CLOCK_PROCESS_CPUTIME_ID
[CPT] [Option Start]
The identifier of the CPU-time clock associated with the process making a clock() or timer*() function call. [Option End]
CLOCK_REALTIME
[CX] [Option Start] The identifier of the system-wide clock measuring real time. [Option End]
CLOCK_THREAD_CPUTIME_ID
[TCT] [Option Start]
The identifier of the CPU-time clock associated with the thread making a clock() or timer*() function call. [Option End]
*/

/*
[CX] [Option Start] The <time.h> header shall define the following symbolic constant:

TIMER_ABSTIME
Flag indicating time is absolute. For functions taking timer objects, this refers to the clock associated with the timer.
[Option End]
[XSI] [Option Start] The <time.h> header shall provide a declaration or definition for getdate_err. The getdate_err symbol shall expand to an expression of type int. It is unspecified whether getdate_err is a macro or an identifier declared with external linkage, and whether or not it is a modifiable lvalue. If a macro definition is suppressed in order to access an actual object, or a program defines an identifier with the name getdate_err, the behavior is undefined. [Option End]
*/

char      *asctime(const struct tm *);
char      *asctime_r(const struct tm *, char *);
clock_t    clock(void);
int        clock_getcpuclockid(pid_t, clockid_t *);
int        clock_getres(clockid_t, struct timespec *);
int        clock_gettime(clockid_t, struct timespec *);
int        clock_nanosleep(clockid_t, int, const struct timespec *,
               struct timespec *);
int        clock_settime(clockid_t, const struct timespec *);
char      *ctime(const time_t *);
char      *ctime_r(const time_t *, char *);
double     difftime(time_t, time_t);
struct tm *getdate(const char *);
struct tm *gmtime(const time_t *);
struct tm *gmtime_r(const time_t *, struct tm *);
struct tm *localtime(const time_t *);
struct tm *localtime_r(const time_t *, struct tm *);
time_t     mktime(struct tm *);
int        nanosleep(const struct timespec *, struct timespec *);
size_t     strftime(char *, size_t, const char *,
           const struct tm *);
size_t     strftime_l(char *, size_t, const char *,
               const struct tm *, locale_t);
char      *strptime(const char *, const char *,
               struct tm *);
time_t     time(time_t *);
int        timer_create(clockid_t, struct sigevent *,
               timer_t *);
int        timer_delete(timer_t);
int        timer_getoverrun(timer_t);
int        timer_gettime(timer_t, struct itimerspec *);
int        timer_settime(timer_t, int, const struct itimerspec *,
               struct itimerspec *);
void       tzset(void);


extern int    daylight;
extern long   timezone;
extern char  *tzname[TZNAME_MAX+1];

#endif
