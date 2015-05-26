#ifndef _SYS_THREADLOCAL_H
#define _SYS_THREADLOCAL_H

#include <time.h>

struct threadlocal {
  // errno must be first.
  int errno;
  pid_t thread_id;
  struct tm tm;
};

// This depends on THREAD_LOCAL_PAGE in "kernel.h"
#define THREADLOCAL ((struct threadlocal*)0x402000)

#endif
