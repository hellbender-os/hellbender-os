#include <kernel/syscall.h>

void syscall_notify(volatile unsigned *obj) {
  (void)(obj); //TODO: unblock the thread here, not in scheduler loop..
}
