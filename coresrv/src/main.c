#include <hellbender/debug.h>
#include "vga.h"

int main(int argc, char* argv[]) {
  VGA_AT(0,9) = VGA_ENTRY('r', WHITE_ON_BLACK);
  (void)argc;
  (void)argv;
  BREAK;
}
