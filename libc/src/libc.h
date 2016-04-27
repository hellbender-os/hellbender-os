#ifndef __LIBC_H__
#define __LIBC_H__

#include <stdint.h>
#include <pthread.h>

struct libc {
  pthread_mutex_t mutex;

  uint64_t process_id;
  uint64_t thread_id;

  int argc;
  char **argv;

  void* threadlocal_init;    // initialization data for thread locals.
  void* threadlocal_base;    // where thread locals are actually mapped.
  uint64_t threadlocal_size; // size of the thread local data.
};

#endif
