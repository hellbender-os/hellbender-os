#include <stdlib.h>
#include <hellbender/hellbender.h>

extern void _fini();

typedef void (*func_ptr)(void);

struct exit_list {
  struct exit_list *next;
  func_ptr func;
};
static struct exit_list *exit_list_head = NULL;

int atexit (void (*func)(void)) {
  struct exit_list *item = (struct exit_list *)malloc(sizeof(struct exit_list));
  if (item) {
    item->func = func;
    item->next = exit_list_head;
    exit_list_head = item;
    return 0;
  } else {
    return -1;
  }
}

void exit(int status) {
  for (struct exit_list *item = exit_list_head; item; ) {
    (*item->func)();
    struct exit_list *next_item = item->next;
    free(item);
    item = next_item;
  }
  _fini();
  syscall_exit(status);
}

void _exit(int status) {
  _fini();
  syscall_exit(status);
}

void _Exit(int status) {
  _fini();
  syscall_exit(status);
}

