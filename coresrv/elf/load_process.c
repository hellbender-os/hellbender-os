#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <hellbender.h>

#include <coresrv/elf.h>
#include <kernel/kernel.h>
#include <kernel/thread.h>
#include <kernel/elf32.h>

__IDCIMPL__ void* elf_load_process(IDC_PTR) {
  // TODO: supports argv, envp.
  
  // bootstrap: COREIDC(proc_bootstrap)
  //   open file.
  //   allocate enough memory for the binary.
  //   load file into the memory.
  //   ask kernel to make code from data.
  //   mmap the args, envp; put address in esi, edi.
  //   jump to _start.

  char path[PATH_MAX];
  syscall_current_exec_path(path);
  printf("elf_load_process: %s\n", path);

  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    printf("Could not open '%s'.\n", path);
    abort();
  }
  Elf32_Ehdr elf_header;
  if (read(fd, &elf_header, sizeof(elf_header)) != sizeof(elf_header)) {
    printf("Could not load elf header.\n");
    abort();
  }
  if (elf_header.e_ident[EI_MAG0] != ELFMAG0 ||
      elf_header.e_ident[EI_MAG1] != ELFMAG1 ||
      elf_header.e_ident[EI_MAG2] != ELFMAG2 ||
      elf_header.e_ident[EI_MAG3] != ELFMAG3) {
    uint32_t m = (elf_header.e_ident[EI_MAG0] |
                  elf_header.e_ident[EI_MAG1] << 8 |
                  elf_header.e_ident[EI_MAG2] << 16 |
                  elf_header.e_ident[EI_MAG3] << 24);
    printf("Wrong elf magic %x.\n", (unsigned)m);
    abort();
  }
  if (elf_header.e_type != ET_EXEC) {
    printf("This elf cannot be executed.\n");
    abort();
  }
  if (elf_header.e_machine != EM_386 && elf_header.e_machine != EM_486) {
    printf("Wrong kind of an elf: %u.\n", (unsigned)elf_header.e_machine);
    abort();
  }
  printf("Elf entry point = %x\n", (unsigned)elf_header.e_entry);

  void *min_addr = syscall_set_program_break(NULL, 0);
  void *max_addr = min_addr;
  Elf32_Phdr prog_header;
  for (unsigned i = 0; i < elf_header.e_phnum; ++i) {
    lseek(fd, elf_header.e_phoff + i * elf_header.e_phentsize, SEEK_SET);
    if (read(fd, &prog_header, sizeof(prog_header)) != sizeof(prog_header)) {
      printf("Could not load elf program header.\n");
      abort();
    }
    //printf("Phdr: .type=%x; .offset=%x; .vaddr=%x; .paddr=%x; "
    //       ".filesz=%x; .memsz=%x; .flags=%x; .align=%x\n",
    //       (unsigned)prog_header.p_type, (unsigned)prog_header.p_offset,
    //       (unsigned)prog_header.p_vaddr, (unsigned)prog_header.p_paddr,
    //       (unsigned)prog_header.p_filesz, (unsigned)prog_header.p_memsz,
    //       (unsigned)prog_header.p_flags, (unsigned)prog_header.p_align);
    if (prog_header.p_type != PT_LOAD) continue;

    // allocate physical memory for each segment in correct virtual address.
    void *vaddr = (void*)prog_header.p_vaddr;
    if (vaddr < min_addr) {
      printf("Cannot map program below virtual address space.\n");
      abort();
    }
    void *vaddr_end = vaddr + prog_header.p_memsz;
    if (vaddr_end > max_addr) {
      syscall_set_program_break(vaddr_end, 0);
      max_addr = vaddr_end;
    }
    memset(vaddr, 0, (size_t)prog_header.p_memsz);

    // load the segment.
    lseek(fd, (off_t)prog_header.p_offset, SEEK_SET);
    if (read(fd, (void*)prog_header.p_vaddr, (size_t)prog_header.p_filesz)
        != prog_header.p_filesz) {
      printf("Failed to load elf program.\n");
      abort();
    }
    //printf("Read data from %x - %x\n",
    //       (unsigned)((off_t)prog_header.p_offset),
    //       (unsigned)((off_t)prog_header.p_offset + prog_header.p_filesz));

    // re-map with correct permissions.
    if (prog_header.p_flags & PF_X) {
      syscall_make_executable((void*)prog_header.p_vaddr,
                              (size_t)prog_header.p_memsz);
    } else if ((prog_header.p_flags & PF_W) == 0) {
      syscall_make_readonly((void*)prog_header.p_vaddr,
                            (size_t)prog_header.p_memsz);
    }
  }

  // process bootstrapper will jump to this address when we return:
  return (void*)elf_header.e_entry;
}
