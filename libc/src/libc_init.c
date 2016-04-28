#include "libc.h"

#include <hellbender/libc_init.h>

#include <pthread.h>
#include <stdint.h>
#include <string.h>

static struct libc libc;

void _libc_init_thread_locals() {
  memcpy(libc.init->threadlocal_base, 
         libc.init->threadlocal_init,
         libc.init->threadlocal_size);
}

struct libc* _libc_init(struct libc_init *init) {
  libc.mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  libc.init = init;
  _libc_init_thread_locals();

  return &libc;
}

struct libc_init *_libc_init_get() {
  return libc.init;
}

