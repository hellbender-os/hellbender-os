#ifndef __HELLBENDER_DEBUG_H__
#define __HELLBENDER_DEBUG_H__

#ifndef ASM
#define BREAK asm( "xchgw %bx, %bx" )
#else
#define BREAK xchgw %bx, %bx
#endif

#endif
