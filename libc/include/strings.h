#ifndef _STRINGS_H
#define _STRINGS_H

#include <locale.h> // nonconforming: locale_t
#include <sys/types.h> // nonconforming: size_t

// The Open Group Base Specifications Issue 7

int    ffs(int);
int    strcasecmp(const char *, const char *);
int    strcasecmp_l(const char *, const char *, locale_t);
int    strncasecmp(const char *, const char *, size_t);
int    strncasecmp_l(const char *, const char *, size_t, locale_t);

#define ffs(v) __builtin_ffs(v)

#endif
