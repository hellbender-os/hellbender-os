#ifndef _SETJMP_H
#define _SETJMP_H

// The Open Group Base Specifications Issue 7

typedef void* jmp_buf[6];
typedef void* sigjmp_buf[8];

void   _longjmp(jmp_buf, int);
void   longjmp(jmp_buf, int);
void   siglongjmp(sigjmp_buf, int);
int    _setjmp(jmp_buf);
int    setjmp(jmp_buf);
int    sigsetjmp(sigjmp_buf, int);

// Hack around GCC builtins limitation for the second argument:
extern __thread int __hellbender_jmpval;

#define _longjmp(_E, _V)   { __hellbender_jmpval = _V; __builtin__longjmp(_E, 1); }
#define longjmp(_E, _V)    { __hellbender_jmpval = _V; __builtin_longjmp(_E, 1); }
#define siglongjmp(_E, _V) { __hellbender_jmpval = _V; __builtin_siglongjmp(_E, 1); }
#define _setjmp(_E)       (__builtin__setjmp(_E) ? __hellbender_jmpval : 0)
#define setjmp(_E)        (__builtin_setjmp(_E) ? __hellbender_jmpval : 0)
#define sigsetjmp(_E, _M) (__builtin_sigsethmp(_E, _M) ? __hellbender_jmpval : 0)

#endif
