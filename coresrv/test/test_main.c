#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <hellbender.h>
#include <coresrv/rtc.h>
#include <coresrv/kbd.h>
#include <coresrv/vga.h>

void test_timer() {
  unsigned counter = 0;
  unsigned last = 0;
  while (counter < 200) {
    counter = coresrv_rtc_ticks();
    if (counter == last) {
      sched_yield();
      continue;
    }
    last = counter;
    if (counter % 20 == 0) {
      printf("tick\n");
    }
  }
}

void read_command(char *buf, size_t size) {
  uint8_t color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
  unsigned x = 2;
  unsigned y = 24;
  unsigned n = 0;
  unsigned max_n = 80 - x;
  if (size < max_n + 1) abort();

  // prompt, and clear the line.
  coresrv_vga_putc(0, y, '>', color);
  for (unsigned i = 1; i < 80; ++i) coresrv_vga_putc(i, y, ' ', color);

  // collect key presses until new-line.
  while (1) {
    int c = coresrv_kbd_getc();
    if (c < 0) {
      sched_yield();
      continue;
    }
    if (c == '\b') {
      if (n) {
        --n;
        coresrv_vga_putc(x + n, y, ' ', color);
      }
      continue;
    }
    if (c == '\n') {
      buf[n] = '\0';
      return;
    }
    if (n < max_n) {
      coresrv_vga_putc(x + n, y, c, color);
      buf[n] = c;
      ++n;
      continue;
    }
  }
}

void test_shell() {
  int running = 1;
  printf("Test shell.\n");
  while (running) {
    char buffer[200];
    read_command(buffer, 200);
  }
}

int main(void) {
  printf("Hello, user World!\n");
  test_shell();
  return 0xabba;
}
