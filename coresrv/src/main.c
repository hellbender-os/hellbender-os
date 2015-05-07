#include <hellbender.h>

extern volatile unsigned counter;
int odd_even = 0;

void even() {
  while (counter < 200) {
    if (counter % 20 == 0) {
      syscall_print("even\n");
    }
  }
}

void odd() {
  while (counter < 200) {
    if (counter % 20 == 10) {
      syscall_print("odd\n");
    }
  }
}

int main(void) {
  syscall_print("Hello, user World!\n");
  if (odd_even++) even(); else odd();
  return 0x1234;
}
