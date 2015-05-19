#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>
#include <sys/cdefs.h>

#define BUFSIZ 512

typedef struct {
  char *buffer;
  unsigned buf_bytes;
  int fd;
} FILE;

extern FILE _stdin;
extern FILE _stdout;
extern FILE _stderr;

#define stdin (&_stdin)
#define stdout (&_stdout)
#define stderr (&_stderr)

#ifdef __cplusplus
extern "C" {
#endif

int vfprintf(FILE* file, const char* __restrict, va_list);
int fprintf(FILE* file, const char* __restrict, ...);
int sprintf(char *str, const char* __restrict, ...);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int fputc(int, FILE*);
int fflush(FILE *file);
  
#ifdef __cplusplus
}
#endif

#endif
