#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/domain.h>

#include <hellbender/hellbender.h>
#include <hellbender/heap.h>

static void* init_heap();

#if !defined(__is_hellbender_kernel)

int _argc;
char** _argv;
extern char** environ;
extern char** _environ_top;
extern char* _environ_static_top;

static void init_args(int argc, int envc, char* ptr);

#endif

/**
 * Initializes all libC features that don't require core services.
 * These feature can be used in libK, and while initializing core services.
 *
 * Basic init for libK cannot use syscalls, it has to do direct access.
 */
void _hellbender_libc_init_basic() {
#if !defined(__is_hellbender_kernel)
  // arguments and environment need to be mapped before heap is initialized.
  int argc, envc;
  char* ptr;
  syscall_get_environment(&argc, &envc, &ptr);
#endif
  
  // malloc, free, realloc.
  void* bottom = init_heap();

#if !defined(__is_hellbender_kernel)
  // _argc, _argv, environ
  _environ_static_top = (char*) bottom;
  init_args(argc, envc, ptr);
#else
  (void)(bottom);
#endif
}

static void* init_heap() {
  // get the heap address space.
#if defined(__is_hellbender_kernel)
  void* bottom = (void*)domain_set_break(&kernel_domain, NULL, 0);
#else
  void* bottom = syscall_set_program_break(NULL, 0);
#endif
 
  // init the malloc heap.
  heap_init_wilderness(&default_wilderness, bottom, 
                       HEAP_DEFAULT_ALLOCATION_SIZE);
  heap_init_large(&default_largeheap, &default_wilderness);
  heap_init_small(&default_smallheap, &default_largeheap);
  heap_init_tiny(&default_tinyheap, &default_largeheap);

  return bottom;
}

#if !defined(__is_hellbender_kernel)
static void init_args(int argc, int envc, char* ptr) {
  _argc = argc;
  _argv = (char**)calloc(argc + 1, sizeof(char*));
  environ = (char**)calloc(envc + 1, sizeof(char*));
  _environ_top = environ + envc;

  // ptr contains all strings, separated by nulls.
  for (int i = 0; i < argc; ++i) {
    _argv[i] = ptr;
    while (*ptr++);
  }
  _argv[argc] = 0;
  
  for (int i = 0; i < envc + 1; ++i) {
    environ[i] = strdup(ptr); // using a copy so that setenv can reallocate.
    while (*ptr++);
  }
  environ[envc] = 0;
}
#endif
