#include <stdio.h>
#include <stdlib.h>

void fcntl_init();
void stdio_init();

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
}
