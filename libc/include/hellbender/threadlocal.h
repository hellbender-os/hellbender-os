#ifndef _SYS_THREADLOCAL_H
#define _SYS_THREADLOCAL_H

#include <time.h>
#include <stdint.h>

struct threadlocal {
  // these must be in the same order as in threadlocal.S
  int errno;
  char *optarg;
  int opterr, optind, optopt;

  // these can be in any order:
  pid_t thread_id;
  struct tm tm;
  char *getopt_optchar;
  char *strtok_saveptr;
  volatile int jmpval;

  // TODO: actions should be in thread heap.
  void (*signal_wrapper)(int, siginfo_t*, void*, struct sigaction*);
  struct sigaction signal_actions[32]; // actions set for signals.
  uint32_t signal_pending; // triggered, not yet processed signals.
  uint32_t signal_mask;    // mask of signals that are not to be processed.
};

// This depends on THREAD_LOCAL_PAGE in "kernel.h"
#define THREADLOCAL ((struct threadlocal*)0x402000)

#endif
