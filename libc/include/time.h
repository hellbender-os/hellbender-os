#ifndef _TIME_H
#define _TIME_H

#include <signal.h>
#include <stdlib.h> // noncorming include
#include <sys/types.h> // noncorming include

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

//#define NULL
// Null pointer constant.
#define CLOCKS_PER_SEC 1000000
//A number used to convert the value returned by the clock() function into seconds.
//CLOCK_PROCESS_CPUTIME_ID
//The identifier of the CPU-time clock associated with the process making a clock() or timer*() function call. [Option End]
//CLOCK_THREAD_CPUTIME_ID
//The identifier of the CPU-time clock associated with the thread making a clock() or timer*() function call. [Option End]

struct timespec {
  time_t  tv_sec; //    Seconds. 
  long    tv_nsec; //   Nanoseconds. 
};

struct itimerspec {
  struct timespec  it_interval; //  Timer period. 
  struct timespec  it_value; //     Timer expiration. 
};
  
//CLOCK_REALTIME
//The identifier of the system-wide realtime clock.
//TIMER_ABSTIME
//Flag indicating time is absolute. For functions taking timer objects, this refers to the clock associated with the timer. [Option End]
//CLOCK_MONOTONIC
//The identifier for the system-wide monotonic clock, which is defined as a clock whose value cannot be set via clock_settime() and which cannot have backward clock jumps. The maximum possible clock jump shall be implementation-defined. [Option End]

//clock_t, size_t, time_t, clockid_t, timer_t from <sys/types.h>.

extern int getdate_err;

char      *asctime(const struct tm *);
char      *asctime_r(const struct tm *restrict, char *restrict);
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
struct tm *gmtime_r(const time_t *restrict, struct tm *restrict);
struct tm *localtime(const time_t *);
struct tm *localtime_r(const time_t *restrict, struct tm *restrict);
time_t     mktime(struct tm *);
int        nanosleep(const struct timespec *, struct timespec *);
size_t     strftime(char *restrict, size_t, const char *restrict,
           const struct tm *restrict);
char      *strptime(const char *restrict, const char *restrict,
               struct tm *restrict);
time_t     time(time_t *);
int        timer_create(clockid_t, struct sigevent *restrict,
                        timer_t *restrict);
int        timer_delete(timer_t);
int        timer_gettime(timer_t, struct itimerspec *);
int        timer_getoverrun(timer_t);
int        timer_settime(timer_t, int, const struct itimerspec *restrict,
               struct itimerspec *restrict);
void       tzset(void);

extern int    daylight;
extern long   timezone;
extern char  *tzname[];

#endif
