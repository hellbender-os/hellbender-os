#include <stdint.h>
#include <coresrv/kbd.h>

int _idc_coresrv_kbd_getc(__attribute__((unused))uintptr_t retval) {
  return 'a';
}

