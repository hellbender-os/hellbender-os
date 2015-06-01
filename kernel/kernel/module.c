#include <stdio.h>
#include <stdlib.h>
#include <kernel/module.h>
#include <kernel/elf32.h>

int module_parse_elf(module_binary_t *binary, kernel_module_t *module) {
  Elf32_Ehdr *elf_header = (Elf32_Ehdr*)binary->bottom;
  if (elf_header->e_ident[EI_MAG0] != ELFMAG0 ||
      elf_header->e_ident[EI_MAG1] != ELFMAG1 ||
      elf_header->e_ident[EI_MAG2] != ELFMAG2 ||
      elf_header->e_ident[EI_MAG3] != ELFMAG3) {
    uint32_t m = (elf_header->e_ident[EI_MAG0] |
                  elf_header->e_ident[EI_MAG1] << 8 |
                  elf_header->e_ident[EI_MAG2] << 16 |
                  elf_header->e_ident[EI_MAG3] << 24);
    printf("Wrong elf magic %x.\n", (unsigned)m);
    abort();
  }
  if (elf_header->e_type != ET_EXEC) {
    printf("This elf cannot be executed.\n");
    abort();
  }
  if (elf_header->e_machine != EM_386 && elf_header->e_machine != EM_486) {
    printf("Wrong kind of an elf: %u.\n", (unsigned)elf_header->e_machine);
    abort();
  }

  // scan all program headers for text and data:
  // TODO: not needed since ELF is parsed at domain.c.
  module->bottom = UINT32_MAX;
  module->top = 0;
  module->ro_bottom = UINT32_MAX;
  module->ro_top = 0;
  module->text_bottom = UINT32_MAX;
  module->text_top = 0;
  //module->start = core_info->module_start;
  //module->module_info = core_info;
  for (unsigned i = 0; i < elf_header->e_phnum; ++i) {
    Elf32_Phdr *prog_header = (Elf32_Phdr*)
      ((uintptr_t)elf_header + elf_header->e_phoff
       + i * elf_header->e_phentsize);
    if (prog_header->p_type != PT_LOAD) continue;
    uint32_t bottom = prog_header->p_vaddr;
    uint32_t top = prog_header->p_vaddr + prog_header->p_memsz;
    if (bottom < module->bottom) module->bottom = bottom;
    if (top > module->top) module->top = top;
    if ((prog_header->p_flags & PF_W) == 0) {
      if (bottom < module->ro_bottom) module->ro_bottom = bottom;
      if (top > module->ro_top) module->ro_top = top;
    }
    if (prog_header->p_flags & PF_X) {
      if (bottom < module->text_bottom) module->text_bottom = bottom;
      if (top > module->text_top) module->text_top = top;
    }
  }
  return 0;
}

