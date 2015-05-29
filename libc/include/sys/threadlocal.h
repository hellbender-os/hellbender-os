#ifndef _SYS_THREADLOCAL_H
#define _SYS_THREADLOCAL_H

#include <time.h>

struct threadlocal {
  // these must be in the same order as in threadlocal.S
  int errno;
  char *optarg;
  int opterr, optind, optopt;
  char *_optchar;

  // these can be in any order:
  pid_t thread_id;
  struct tm tm;
};

// This depends on THREAD_LOCAL_PAGE in "kernel.h"
#define THREADLOCAL ((struct threadlocal*)0x402000)

#endif
