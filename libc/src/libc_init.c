#include "libc_init.h"
#include "libc.h"

#include <pthread.h>
#include <stdint.h>
#include <string.h>

static struct libc libc;

void _libc_init_thread_locals() {
  memcpy(libc.threadlocal_base, libc.threadlocal_init, libc.threadlocal_size);
}

void _libc_init_thread(void* tl_base, void* tl_init, uint64_t tl_size,
                       uint64_t zero, uint64_t process_id, uint64_t thread_id) {
  (void)zero;
  libc.mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  libc.process_id = process_id;
  libc.thread_id = thread_id;

  // save thread local data for later use (if more threads are created).
  libc.threadlocal_base = tl_base;
  libc.threadlocal_init = tl_init;
  libc.threadlocal_size = tl_size;
  _libc_init_thread_locals();
}

struct libc* _libc_init_args(char* argv, void* stack_top) {
  (void)argv;
  (void)stack_top;
  // TODO: scan argc, envp
  return &libc;
}
