#ifndef __CORESRV_INIT_H__
#define __CORESRV_INIT_H__

#include <stddef.h>

struct coresrv_init {
  void* vga_base;
  size_t vga_size;

  void* initrd_base;
  size_t initrd_size;
};

#endif
