#include "heap.h"
#include <pthread.h>

struct heap {
};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct heap global_heap;

static void* do_alloc(struct heap* heap, size_t size) {
  (void)heap; // TODO implement
  (void)size;
  return 0;
}

static void do_free(struct heap* heap, void* ptr) {
  (void)heap; // TODO implement
  (void)ptr;
}

void* _heap_alloc(size_t size) { 
  pthread_mutex_lock(&mutex);
  void* ptr = do_alloc(&global_heap, size);
  pthread_mutex_unlock(&mutex);
  return ptr;
}

void _heap_free(void* ptr) {
  pthread_mutex_lock(&mutex);
  do_free(&global_heap, ptr);
  pthread_mutex_unlock(&mutex);
}
