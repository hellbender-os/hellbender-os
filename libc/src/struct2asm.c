#include "libc.h"
#include <stdint.h>

/* magic macros -- don't change these */
#define OFFSETOF(TYPE, MEMBER) ((uintptr_t) &((struct TYPE *)0)->MEMBER)
#define SIZEOF(TYPE, MEMBER) (sizeof(((struct TYPE *)0)->MEMBER))
 
#define _DEFINE(sym, val) asm volatile("\n-> " #sym " %0 " #val "\n" : : "i" (val))
#define STRUCT(s) \
	_DEFINE(sizeof_##s, sizeof(struct s));
#define MEMBER(s, m) \
	_DEFINE(offsetof_##s##_##m, OFFSETOF(s, m)); \
	_DEFINE(sizeof_##s##_##m, SIZEOF(s, m));
 
/* function with your structures and members */
foo() {
  STRUCT(libc);
  MEMBER(libc, argc);
  MEMBER(libc, argv);
}
