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

// Using GCC builtins:
#define _longjmp __builtin_longjmp
#define longjmp __builtin_longjmp
#define siglongjmp __builtin_siglongjmp
#define _setjmp __builtin_setjmp
#define setjmp __builtin_setjmp
#define sigsetjmp __builtin_sigsetjmp

#endif
