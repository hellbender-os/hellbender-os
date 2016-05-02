#include <hellbender/dev/console.h>
#include <hellbender/broker.h>
#include <hellbender/syscall.h>

static console_t console;

int console_create(cdev_t *dev) {
  (void)dev;
  return -1;
}

void console_bind(console_t *console_) {
  *console_ = console;
}

void console_init() {
  BROKER_REGISTER(CONSOLE_ID, REGISTER_SERVICE(console_bind));
  console.create = REGISTER_SERVICE(console_create);
}
