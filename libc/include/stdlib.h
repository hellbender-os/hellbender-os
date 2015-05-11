#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NULL
#  undef NULL
#endif
#define NULL ((void*)0)

// string conversion (PARTIALLY SUPPORTED)
  
double atof (const char* str);
int atoi (const char * str);
long int atol ( const char * str );
double strtod (const char* str, char** endptr);
long int strtol (const char* str, char** endptr, int base);
unsigned long int strtoul (const char* str, char** endptr, int base);
char* _itoa(int value, char* str, int base);
char* _utoa(unsigned value, char* str, int base);

// dynamic memory management (SUPPORTED)
  
void* calloc(size_t num, size_t size);
void free(void* ptr);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);

// psudo-random sequence generation (NOT SUPPORTED)

#define RAND_MAX 2147483647
  
void _srand(); // uses entropy from kernel to init rand.
void srand(unsigned int seed);
int rand(void);

// environment (SUPPORTED)

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
  
__attribute__((__noreturn__)) void abort(void);
int atexit (void (*func)(void));
__attribute__((__noreturn__)) void exit(int status);
char* getenv (const char* name);
int system (const char* command);

// searching and sorting (SUPPORTED)

void* bsearch (const void* key, const void* base,
               size_t num, size_t size,
               int (*compar)(const void*,const void*));
void qsort (void* base, size_t num, size_t size,
            int (*compar)(const void*,const void*));
  
// integer arithmetics (SUPPORTED)

typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long int quot;
  long int rem;
} ldiv_t;

int abs (int n);
div_t div (int numer, int denom);
long int labs (long int n);
ldiv_t ldiv (long int numer, long int denom);
  
// multibyte characters (NOT SUPPORTED)

#define MB_CUR_MAX 0
  
int mblen (const char* pmb, size_t max);
int mbtowc (wchar_t* pwc, const char* pmb, size_t max);
int wctomb (char* pmb, wchar_t wc);
  
#ifdef __cplusplus
}
#endif

#endif
