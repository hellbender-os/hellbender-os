#ifndef _STDLIB_H
#define _STDLIB_H

//The Open Group Base Specifications Issue 7

#include <stddef.h>
#include <limits.h>
#include <math.h>
#include <sys/wait.h>

#define EXIT_FAILURE 1
//Unsuccessful termination for exit(); evaluates to a non-zero value.

#define EXIT_SUCCESS 0
//Successful termination for exit(); evaluates to 0.

#define RAND_MAX 2147483647
//Maximum value returned by rand(); at least 32767.

#define MB_CUR_MAX 4
//Maximum number of bytes in a character specified by the current locale (category LC_CTYPE).

//NULL as described in <stddef.h>.

typedef struct {
  int quot;
  int rem;
} div_t;
//Structure type returned by the div() function.

typedef struct {
  long int quot;
  long int rem;
} ldiv_t;
//Structure type returned by the ldiv() function.

typedef struct {
  long long int quot;
  long long int rem;
} lldiv_t;
//Structure type returned by the lldiv() function.

//size_t As described in <stddef.h>.
//wchar_t As described in <stddef.h>.

/*the following symbolic constants and macros as described in <sys/wait.h>:

WEXITSTATUS
WIFEXITED
WIFSIGNALED
WIFSTOPPED
WNOHANG
WSTOPSIG
WTERMSIG
WUNTRACED
*/

void          _Exit(int) __attribute__((noreturn));
long          a64l(const char *);
void          abort(void) __attribute__((noreturn));
int           abs(int);
int           atexit(void (*)(void));
double        atof(const char *);
int           atoi(const char *);
long          atol(const char *);
long long     atoll(const char *);
void         *bsearch(const void *, const void *, size_t, size_t,
                  int (*)(const void *, const void *));
void         *calloc(size_t, size_t);
div_t         div(int, int);
double        drand48(void);
double        erand48(unsigned short [3]);
void          exit(int) __attribute__((noreturn));
void          free(void *);
char         *getenv(const char *);
int           getsubopt(char **, char *const *, char **);
int           grantpt(int);
char         *initstate(unsigned, char *, size_t);
long          jrand48(unsigned short [3]);
char         *l64a(long);
long          labs(long);
void          lcong48(unsigned short [7]);
ldiv_t        ldiv(long, long);
long long     llabs(long long);
lldiv_t       lldiv(long long, long long);
long          lrand48(void);
void         *malloc(size_t);
int           mblen(const char *, size_t);
size_t        mbstowcs(wchar_t *, const char *, size_t);
int           mbtowc(wchar_t *, const char *, size_t);
char         *mkdtemp(char *);
int           mkstemp(char *);
long          mrand48(void);
long          nrand48(unsigned short [3]);
int           posix_memalign(void **, size_t, size_t);
int           posix_openpt(int);
char         *ptsname(int);
//this is insane function: int           putenv(char *);
void          qsort(void *, size_t, size_t, int (*)(const void *,
                  const void *));
int           rand(void);
int           rand_r(unsigned *);
long          random(void);
void         *realloc(void *, size_t);
char         *realpath(const char *, char *);
unsigned short *seed48(unsigned short [3]);
int           setenv(const char *, const char *, int);
void          setkey(const char *);
char         *setstate(char *);
void          srand(unsigned);
void          srand48(long);
void          srandom(unsigned);
double        strtod(const char *, char **);
float         strtof(const char *, char **);
long          strtol(const char *, char **, int);
long double   strtold(const char *, char **);
long long     strtoll(const char *, char **, int);
unsigned long strtoul(const char *, char **, int);
unsigned long long
              strtoull(const char *, char **, int);
int           system(const char *);
int           unlockpt(int);
int           unsetenv(const char *);
size_t        wcstombs(char *, const wchar_t *, size_t);
int           wctomb(char *, wchar_t);

// nonconforming:

extern const char _ldigits[]; // lowercase digits 0-z.
extern const char _udigits[]; // uppercase digits 0-Z.

// negative bases use uppercase digits.
char* _itoa_n(int value, char* str, size_t size, int base); 
char* _lltoa_n(long long value, char* str, size_t size, int base);
char* _utoa_n(unsigned value, char* str, size_t size, int base);
char* _ulltoa_n(unsigned long long value, char* str, size_t size, int base);

#endif
