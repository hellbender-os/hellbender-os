#include <stdio.h>
#include <hellbender/hellbender.h>
#include <coresrv/ps2.h>
#include <coresrv/kbd.h>

extern void kbd_isr();

void ps2_chan1_isr() {
  switch (ps2.chan1_mode) {
  case PS2_MODE_KEYBOARD:
    kbd_isr();
    break;
  default:
    // unsupported
    break;
  }
  syscall_iret();
}

void ps2_chan2_isr() {
  switch (ps2.chan2_mode) {
  case PS2_MODE_KEYBOARD:
    kbd_isr();
    break;
  default:
    // unsupported
    break;
  }
  syscall_iret();
}
