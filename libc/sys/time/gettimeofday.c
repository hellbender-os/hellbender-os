#include <sys/time.h>
#include <time.h>
#include <stddef.h>

int gettimeofday(struct timeval *tp, void *tzp) {
  (void)(tzp);
  if (tp) {
    // TODO: use clock_gettime
    tp->tv_sec = time(NULL);
    tp->tv_usec = 0;
  }
  return 0;
}
