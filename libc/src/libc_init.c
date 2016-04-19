#include "libc_init.h"

#include <stdint.h>
#include <string.h>

uint64_t _process_id;
uint64_t _thread_id;

int _argc;
char **_argv;

static void* _threadlocal_init; // initialization data for thread locals.
static void* _threadlocal_base; // where thread locals are actually mapped.
static uint64_t _threadlocal_size; // size of the thread local data.

void _libc_init_thread_locals() {
  memcpy(_threadlocal_base, _threadlocal_init, _threadlocal_size);
}

void _libc_init_thread(void* tl_base, void* tl_init, uint64_t tl_size,
                       uint64_t zero, uint64_t process_id, uint64_t thread_id) {
  (void)zero;
  _process_id = process_id;
  _thread_id = thread_id;

  // save thread local data for later use (if more threads are created).
  _threadlocal_base = tl_base;
  _threadlocal_init = tl_init;
  _threadlocal_size = tl_size;
  _libc_init_thread_locals();
}

void _libc_init_args(char* argv, void* stack_top) {
  (void)argv;
  (void)stack_top;
  // TODO: scan argc, envp
}
