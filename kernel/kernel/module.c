#include <kernel/module.h>
#include <kernel/kstdio.h>

int module_check_header(kernel_module_t *module) {
  uint16_t *fields = (uint16_t*)module;
  uint32_t sum = 0;
  unsigned i_end = sizeof(kernel_module_t)/sizeof(uint16_t) - 2;
  for (unsigned i = 0; i < i_end; ++i) {
    sum += fields[i];
  }
  return sum == module->checksum;
}
