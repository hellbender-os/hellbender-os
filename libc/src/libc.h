#ifndef __LIBC_H__
#define __LIBC_H__

#include <stdint.h>
#include <pthread.h>

struct libc_init;

struct libc {
  pthread_mutex_t mutex;
  struct libc_init *init;

  int argc;
  char **argv;
};

#endif
