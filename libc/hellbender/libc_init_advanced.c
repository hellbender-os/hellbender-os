#include <stdio.h>
#include <stdlib.h>

#include <sys/keymap.h>

void fcntl_init();
void stdio_init();
keymap_t* keymap;

/**
 * Initializes all libC feature that require core services.
 * These features can be used in userspace applications, and 
 * in core services after core initialization has completed.
 */
void _hellbender_libc_init_advanced() {
  // open, creat, close, read, write, lseek
  fcntl_init();

  // fopen, fclose, fprintf, etc.
  stdio_init();

  // TODO: support actual locales.
  keymap = keymap_create("fi");
}
