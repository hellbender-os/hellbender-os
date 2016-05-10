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
#define MULTIBOOT_FLAGS_ALIGN   (1<<0)
#define MULTIBOOT_FLAGS_MEMINFO (1<<1)
#define MULTIBOOT_FLAGS_GFXINFO (1<<2)
     
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
  uint32_t mode_type;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
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
  uint32_t mem_lower;         //    (present if flags[0] is set)
  uint32_t mem_upper;         //    (present if flags[0] is set)
  uint32_t boot_device;       //    (present if flags[1] is set)
  uint32_t cmdline;           //    (present if flags[2] is set)
  uint32_t mods_count;        //    (present if flags[3] is set)
  uint32_t mods_addr;         //    (present if flags[3] is set)
  union
  {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;                        //    (present if flags[4] or flags[5] is set)
  uint32_t mmap_length;       //    (present if flags[6] is set)
  uint32_t mmap_addr;         //    (present if flags[6] is set)
  uint32_t drives_length;     //    (present if flags[7] is set)
  uint32_t drives_addr;       //    (present if flags[7] is set)
  uint32_t config_table;      //    (present if flags[8] is set)
  uint32_t boot_loader_name;  //    (present if flags[9] is set)
  uint32_t apm_table;         //    (present if flags[10] is set)
  uint32_t vbe_control_info;  //    (present if flags[11] is set)
  uint32_t vbe_mode_info;
  uint32_t vbe_mode;
  uint32_t vbe_interface_seg;
  uint32_t vbe_interface_off;
  uint32_t vbe_interface_len;
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

typedef struct __attribute__((packed)) vbe_control_info {
  uint32_t vbe_signature;        // db 'VESA' VBE Signature
  uint16_t vbe_version;          // dw 0200h or 0300h ; VBE Version
  uint32_t oem_string_ptr;       // dd ? ; Pointer to OEM String
  uint32_t capabilities;         // db 4 dup (?) ; Capabilities of graphics controller
  uint32_t video_mode_ptr;       // dd ? ; Pointer to VideoModeList (uint16_t*)
  uint16_t total_memory;         // dw ? ; Number of 64kb memory blocks
                                 //  ; Added for VBE 2.0
  //uint16_t oem_software_rev;     // dw ? ; VBE implementation Software revision
  //uint32_t oem_vendor_name_ptr;  // dd ? ; Pointer to Vendor Name String
  //uint32_t oem_product_name_ptr; // dd ? ; Pointer to Product Name String
  //uint32_t oem_product_rev_ptr;  // dd ? ; Pointer to Product Revision String
  //uint8_t reserved[222];         // db 222 dup (?) ; VBE implementation scratch area
  //uint8_t oem_data[256];         // db 256 dup (?) ; Data Area for OEM Strings
} vbe_control_info_t;

typedef struct __attribute__((packed)) vbe_mode_info {
  // ; Mandatory information for all VBE revisions
  uint16_t mode_attributes;       // dw ? ; mode attributes
  uint8_t win_a_attributes;       // db ? ; window A attributes
  uint8_t win_b_attributes;       // db ? ; window B attributes
  uint16_t win_granularity;       // dw ? ; window granularity
  uint16_t win_size;              // dw ? ; window size
  uint16_t win_a_segment;         // dw ? ; window A start segment
  uint16_t win_b_segment;         // dw ? ; window B start segment
  uint32_t win_func_ptr;          // dd ? ; pointer to window function
  uint16_t bytes_per_scanline;    // dw ? ; bytes per scan line
  // ; Mandatory information for VBE 1.2 and above
  uint16_t x_resolution;          // dw ? ; horizontal resolution in pixels or characters
  uint16_t y_resolution;          // dw ? ; vertical resolution in pixels or characters
  uint8_t x_char_size;            // db ? ; character cell width in pixels
  uint8_t y_char_size;            // db ? ; character cell height in pixels
  uint8_t number_of_planes;       // db ? ; number of memory planes
  uint8_t bits_per_pixel;         // db ? ; bits per pixel
  uint8_t number_of_banks;        // db ? ; number of banks
  uint8_t memory_model;           // db ? ; memory model type
  uint8_t bank_size;              // db ? ; bank size in KB
  uint8_t number_of_image_pages;  // db ? ; number of images
  uint8_t reserved_1;             // db 1 ; reserved for page function
  // ; Direct Color fields (required for direct/6 and YUV/7 memory models)
  uint8_t red_mask_size;          // db ? ; size of direct color red mask in bits
  uint8_t red_field_position;     // db ? ; bit position of lsb of red mask
  uint8_t green_mask_size;        // db ? ; size of direct color green mask in bits
  uint8_t green_field_position;   // db ? ; bit position of lsb of green mask
  uint8_t blue_mask_size;         // db ? ; size of direct color blue mask in bits
  uint8_t blue_field_position;    // db ? ; bit position of lsb of blue mask
  uint8_t rsvd_mask_size;         // db ? ; size of direct color reserved mask in bits
  uint8_t rsvd_field_position;    // db ? ; bit position of lsb of reserved mask
  uint8_t direct_color_mode_info; // db ? ; direct color mode attributes
  // ; Mandatory information for VBE 2.0 and above
  uint32_t phys_base_ptr;         // dd ? ; physical address for flat memory frame buffer
  uint32_t offscreen_mem_offset;  // dd ? ; pointer to start of off screen memory
  uint16_t offscreen_mem_size;    // dw ? ; amount of off screen memory in 1k units
  //uint8_t reserved_2;             // db 206 dup (?) ; remainder of ModeInfoBlock
} vbe_mode_info_t;

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

  vbe_control_info_t vbe_info;
  vbe_mode_info_t mode_info;
};

extern struct multiboot_data multiboot_data;

#define MULTIBOOT_TOO_MANY_MEMORY_MAPS -1
#define MULTIBOOT_TOO_MANY_MODULES     -2
#define MULTIBOOT_TOO_MANY_ELF_HEADERS -3

// return 0 on success; <0 on error.
int multiboot_copy(multiboot_info_t *info);
     
#endif /* ! ASM */

#endif
