#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <hellbender.h>

#include <coresrv/elf.h>
#include <kernel/kernel.h>
#include <kernel/thread.h>

__IDCIMPL__ void* elf_load_process(IDC_PTR) {
  char path[PATH_MAX];
  syscall_current_exec_path(path);
  printf("elf_load_process: %s\n", path);
  
  // bootstrap: COREIDC(proc_bootstrap)
  //   open file.
  //   allocate enough memory for the binary.
  //   load file into the memory.
  //   ask kernel to make code from data.
  //   allocate enough memory for data.
  //   initialize data.
  //   mmap the args, envp; put address in esi, edi.
  //   jump to _start.
  return NULL;
}
