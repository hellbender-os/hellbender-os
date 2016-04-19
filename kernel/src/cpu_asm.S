#include "config.h"

        /* CPU specific page is accessed through the recursive page mapping.
         * This way we don't to allocate intermediate tables for each CPU.
         */
        .global cpu
        .equ cpu, CPU_PAGE_ADDRESS

