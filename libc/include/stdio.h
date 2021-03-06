#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h> // nonconforming: for va_args
#include <sys/types.h> // nonconforming for off_t, ssize_t
#include <hellbender/list.h>
#include <hellbender/stdio_impl.h>

// The Open Group Base Specifications Issue 7

#include <stddef.h>

typedef struct _FILE FILE;
// A structure containing information about a file.

typedef unsigned fpos_t;
//A non-array type containing all information needed to specify uniquely every position within a file.

//off_t As described in <sys/types.h>.
//size_t As described in <stddef.h>.
//ssize_t As described in <sys/types.h>.
//va_list As described in <stdarg.h>.

#define BUFSIZ 512
//Size of <stdio.h> buffers. This shall expand to a positive value.
//L_ctermid
//Maximum size of character array to hold ctermid() output.
//L_tmpnam
//Maximum size of character array to hold tmpnam() output.

#define _IOFBF  1
//Input/output fully buffered.
#define _IOLBF  2
//Input/output line buffered.
#define _IONBF  3
//Input/output unbuffered.

#define SEEK_CUR 1
//Seek relative to current position.
#define SEEK_END 2
//Seek relative to end-of-file.
#define SEEK_SET 3
//Seek relative to start-of-file.

#define FILENAME_MAX 256
//Maximum size in bytes of the longest pathname that the implementation guarantees can be opened.
#define FOPEN_MAX 20
//Number of streams which the implementation guarantees can be open simultaneously. The value is at least eight.
//TMP_MAX
//Minimum number of unique filenames generated by tmpnam(). Maximum number of times an application can call tmpnam() reliably. The value of {TMP_MAX} is at least 25.
//On XSI-conformant systems, the value of {TMP_MAX} is at least 10000.

#define EOF (-1)
//End-of-file return value.

//NULL as described in <stddef.h>.

#define P_tmpdir "tmp"
//Default directory prefix for tempnam().

#define stderr (&_stdio._stderr)
//Standard error output stream.
#define stdin (&_stdio._stdin)
//Standard input stream.
#define stdout (&_stdio._stdout)
//Standard output stream.

void     clearerr(FILE *);
char    *ctermid(char *);
int      dprintf(int, const char *, ...);
int      fclose(FILE *);
FILE    *fdopen(int, const char *);
int      feof(FILE *);
int      ferror(FILE *);
int      fflush(FILE *);
int      fgetc(FILE *);
int      fgetpos(FILE *, fpos_t *);
char    *fgets(char *, int, FILE *);
int      fileno(FILE *);
void     flockfile(FILE *);
FILE    *fmemopen(void *, size_t, const char *);
FILE    *fopen(const char *, const char *);
int      fprintf(FILE *, const char *, ...);
int      fputc(int, FILE *);
int      fputs(const char *, FILE *);
size_t   fread(void *, size_t, size_t, FILE *);
FILE    *freopen(const char *, const char *, FILE *);
int      fscanf(FILE *, const char *, ...);
int      fseek(FILE *, long, int);
int      fseeko(FILE *, off_t, int);
int      fsetpos(FILE *, const fpos_t *);
long     ftell(FILE *);
off_t    ftello(FILE *);
int      ftrylockfile(FILE *);
void     funlockfile(FILE *);
size_t   fwrite(const void *, size_t, size_t, FILE *);
#define  getc(FILE) fgetc(FILE)
int      getchar(void);
int      getc_unlocked(FILE *);
int      getchar_unlocked(void);
ssize_t  getdelim(char **, size_t *, int,
             FILE *);
ssize_t  getline(char **, size_t *, FILE *);
char    *gets(char *);
FILE    *open_memstream(char **, size_t *);
int      pclose(FILE *);
void     perror(const char *);
FILE    *popen(const char *, const char *);
int      printf(const char *, ...);
int      putc(int, FILE *);
int      putchar(int);
int      putc_unlocked(int, FILE *);
int      putchar_unlocked(int);
int      puts(const char *);
int      remove(const char *);
int      rename(const char *, const char *);
int      renameat(int, const char *, int, const char *);
void     rewind(FILE *);
int      scanf(const char *, ...);
void     setbuf(FILE *, char *);
int      setvbuf(FILE *, char *, int, size_t);
int      snprintf(char *, size_t, const char *, ...);
int      sprintf(char *, const char *, ...);
int      sscanf(const char *, const char *, ...);
char    *tempnam(const char *, const char *);
FILE    *tmpfile(void);
char    *tmpnam(char *);
int      ungetc(int, FILE *);
int      vdprintf(int, const char *, va_list);
int      vfprintf(FILE *, const char *, va_list);
int      vfscanf(FILE *, const char *, va_list);
int      vprintf(const char *, va_list);
int      vscanf(const char *, va_list);
int      vsnprintf(char *, size_t, const char *, va_list);
int      vsprintf(char *, const char *, va_list);
int      vsscanf(const char *, const char *, va_list);

#endif
