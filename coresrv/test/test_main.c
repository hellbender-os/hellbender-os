#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <hellbender.h>
#include <coresrv/rtc.h>
#include <coresrv/kbd.h>
#include <coresrv/vga.h>
#include <sys/keymap.h>

keymap_t* keymap;

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
  unsigned max_n = 79 - x;
  if (size < max_n + 1) abort();

  // prompt, and clear the line.
  coresrv_vga_putc(0, y, x, '>', color);
  for (unsigned i = 1; i < 80; ++i) coresrv_vga_putc(i, y, x, ' ', color);

  // collect key presses until new-line.
  while (1) {
    int kc = coresrv_kbd_getc();
    if (kc < 0) {
      sched_yield();
      continue;
    }
    int c = keymap_code2char(keymap, KBD_GETC_KEYCODE(kc), KBD_GETC_FLAGS(kc));
    if (c <= 0) continue;
    //if (c <= 0) {
    //  printf("code %x = %c\n", (unsigned)KBD_GETC_KEYCODE(kc), (unsigned)c);
    //  continue;
    //}
    if (c == '\b') {
      if (n) {
        --n;
        coresrv_vga_putc(x + n, y, x + n, ' ', color);
      }
      continue;
    }
    if (c == '\n') {
      buf[n] = '\0';
      return;
    }
    if (n < max_n) {
      coresrv_vga_putc(x + n, y, x + n + 1, c, color);
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
  keymap = keymap_create("fi");
  test_shell();
  return 0xabba;
}
