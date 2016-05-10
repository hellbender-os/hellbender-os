#include "log.h"
#include "vga.h"
#include "kernel.h"

static void log_entry(const char *component, const char *function, const char *message) {
  vga_puts(component);
  vga_putc('/');
  vga_puts(function);
  vga_puts(": ");
  vga_puts(message);
  vga_newl();
}

void log_debug(const char *component, const char *function, const char *message) {
#ifdef DEBUG
  vga_set_fg(VGA_RGB(200,200,200));
  vga_set_bg(VGA_RGB(  0,  0,  0));
  log_entry(component, function, message);
#else
  (void)component;
  (void)function;
  (void)message;
#endif
}

void log_info(const char *component, const char *function, const char *message) {
  vga_set_fg(VGA_RGB(200,200,200));
  vga_set_bg(VGA_RGB(  0,  0,  0));
  log_entry(component, function, message);
}

void log_warn(const char *component, const char *function, const char *message) {
  vga_set_fg(VGA_RGB(  0,  0,  0));
  vga_set_bg(VGA_RGB(200,200,  0));
  log_entry(component, function, message);
}

void log_error(const char *component, const char *function, const char *message) {
  vga_set_fg(VGA_RGB(255,255,255));
  vga_set_bg(VGA_RGB(200,  0,  0));
  log_entry(component, function, message);
  kernel_panic();
  __builtin_unreachable();
}

