#ifndef __LIBC_INIT_H__
#define __LIBC_INIT_H__

#include <stdint.h>

struct libc_init {
  uint64_t process_id;
  uint64_t thread_id;

  void* threadlocal_init;    // initialization data for thread locals.
  void* threadlocal_base;    // where thread locals are actually mapped.
  uint64_t threadlocal_size; // size of the thread local data.

  void* data; // process specific initialization data.
};

struct libc_init *_libc_init_get();

#endif
