#include <stdlib.h>
#include <pthread.h>

extern pthread_mutex_t _libc_mutex;
extern void (*_exit_func)(void);

typedef void (*func_ptr)(void);

struct exit_list {
  struct exit_list *next;
  func_ptr func;
};
static struct exit_list *exit_list_head = NULL;

static void run_atexit() {
  for (struct exit_list *item = exit_list_head; item; ) {
    (*item->func)();
    struct exit_list *next_item = item->next;
    free(item);
    item = next_item;
  }
}

int atexit(void (*func)(void)) {
  pthread_mutex_lock(&_libc_mutex);
  _exit_func = run_atexit;
  struct exit_list *item = (struct exit_list *)malloc(sizeof(struct exit_list));
  if (item) {
    item->func = func;
    item->next = exit_list_head;
    exit_list_head = item;
    pthread_mutex_unlock(&_libc_mutex);
    return 0;
  } else {
    pthread_mutex_unlock(&_libc_mutex);
    return -1;
  }
}
