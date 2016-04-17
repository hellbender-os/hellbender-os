/* multiboot.h - the header for Multiboot */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.
     
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
     
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
     
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */
     
/* Macros. */
#ifndef _KERKEN_MULTIBOOT_H
#define _KERKEN_MULTIBOOT_H

#ifndef ASM
#include "elf32.h"
#include <stdint.h>
#endif

/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002
     
/* The flags for the Multiboot header. */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif
     
/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002
     
/* The size of our stack (16KB). */
#define STACK_SIZE                      0x4000
     
/* C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif
     
#ifndef ASM
/* Do not include here in boot.S. */
     
/* Types. */
     
/* The Multiboot header. */
typedef struct multiboot_header
{
  uint32_t magic;
  uint32_t flags;
  uint32_t checksum;
  uint32_t header_addr;
  uint32_t load_addr;
  uint32_t load_end_addr;
  uint32_t bss_end_addr;
  uint32_t entry_addr;
} multiboot_header_t;
     
/* The symbol table for a.out. */
typedef struct aout_symbol_table
{
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
  uint32_t reserved;
} aout_symbol_table_t;
     
/* The section header table for ELF. */
typedef struct elf_section_header_table
{
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
} elf_section_header_table_t;
     
/* The Multiboot information. */
typedef struct multiboot_info
{
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t boot_device;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  union
  {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;
  uint32_t mmap_length;
  uint32_t mmap_addr;
} multiboot_info_t;
     
/* The module structure. */
typedef struct module
{
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t reserved;
} module_t;
     
/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size. */
typedef struct memory_map
{
  uint32_t size;
  uint32_t base_addr_low;
  uint32_t base_addr_high;
  uint32_t length_low;
  uint32_t length_high;
  uint32_t type;
} memory_map_t;

/* Structure to copy all multiboot stuff into (because otherwise we cannot
 * use any memory due to the possibility of overwriting multiboot data).
 */
#define MAX_MEMORY_MAPS 16
#define MAX_MODULES     16
#define MAX_HEADERS     16
struct multiboot_data {
  memory_map_t mem_maps[MAX_MEMORY_MAPS];
  unsigned n_mem_maps;

  module_t modules[MAX_MODULES];
  unsigned n_modules;
  int coresrv_module;
  int initrd_module;
    
  Elf32_Shdr headers[MAX_HEADERS];
  unsigned n_headers;

  uintptr_t memory_bottom;
  uintptr_t memory_top;
  uintptr_t allocated_top; // anything above this is free.
};

extern struct multiboot_data multiboot_data;

#define MULTIBOOT_TOO_MANY_MEMORY_MAPS -1
#define MULTIBOOT_TOO_MANY_MODULES     -2
#define MULTIBOOT_TOO_MANY_ELF_HEADERS -3

// return 0 on success; <0 on error.
int multiboot_copy(multiboot_info_t *info);
     
#endif /* ! ASM */

#endif
