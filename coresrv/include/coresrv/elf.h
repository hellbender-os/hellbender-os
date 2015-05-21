#ifndef _CORESRV_ELF_H
#define _CORESRV_ELF_H

#include <kernel/idc.h>

/**
 * Loads the binary pointer by CURRENT_THREAD->exec_path.
 * Prepares the DS, CS segments.
 * Sets argc to EDI, argv to ESI.
 * Returns the address to _start.
 */
__IDC__ void* elf_load_process(IDC_PTR);

#endif
