#ifndef _SETJMP_H
#define _SETJMP_H

// The Open Group Base Specifications Issue 7

typedef void* jmp_buf[4];
typedef void* sigjmp_buf[6];

void   _longjmp(jmp_buf, int);
void   longjmp(jmp_buf, int);
void   siglongjmp(sigjmp_buf, int);
int    _setjmp(jmp_buf);
int    setjmp(jmp_buf);
int    sigsetjmp(sigjmp_buf, int);

#endif
