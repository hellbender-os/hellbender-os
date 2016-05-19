#include <sys/time.h>
#include <time.h>
#include <stddef.h>

#include <hellbender/syscall.h>

int gettimeofday(struct timeval *tp, void *tzp) {
  (void)(tzp);
  if (tp) {
    static uint64_t prev_nsec = 0;
    uint64_t nsec;
    do { // just to make sure gettimeofday is monotonic.
      nsec = syscall_timeofday(tp);
    } while (nsec < prev_nsec);
    prev_nsec = nsec;
  }
  return 0;
}
