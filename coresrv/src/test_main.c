#include <stdio.h>
#include <sched.h>
#include <hellbender.h>
#include <coresrv/timer.h>

int main(void) {
  printf("Hello, user World!\n");
  unsigned counter = 0;
  unsigned last = 0;
  while (counter < 200) {
    counter = coresrv_get_ticks();
    if (counter == last) {
      sched_yield();
      continue;
    }
    last = counter;
    if (counter % 20 == 0) {
      printf("tick\n");
    }
  }
  return 0xabba;
}
