#include <stdio.h>
#include <sched.h>
#include <hellbender.h>

int main(void) {
  printf("Hello, user World!\n");
  unsigned counter = 1;// ipc(core_get_ticks);
  while (counter < 200) {
    if (counter % 20 == 0) {
      printf("tick\n");
    }
    sched_yield();
  }
  return 0xabba;
}
