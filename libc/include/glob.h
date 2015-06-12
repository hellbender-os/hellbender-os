#ifndef _GLOB_H
#define _GLOB_H

#include <sys/types.h> // nonconforming: size_t

// The Open Group Base Specifications Issue 7

typedef struct {
  size_t   gl_pathc; // Count of paths matched by pattern. 
  char   **gl_pathv; // Pointer to a list of matched pathnames. 
  size_t   gl_offs;  //  Slots to reserve at the beginning of gl_pathv. 
} glob_t;

#define GLOB_APPEND   0x010
// Append generated pathnames to those previously obtained.

#define GLOB_DOOFFS   0x020
// Specify how many null pointers to add to the beginning of gl_pathv.

#define GLOB_ERR      0x040
// Cause glob() to return on error.

#define GLOB_MARK     0x080
// Each pathname that is a directory that matches pattern has a <slash> appended.

#define GLOB_NOCHECK  0x100
// If pattern does not match any pathname, then return a list consisting of only pattern.

#define GLOB_NOESCAPE 0x004
// Disable backslash escaping.

#define GLOB_NOSORT   0x200
// Do not sort the pathnames returned.

#define GLOB_ABORTED 1
//The scan was stopped because GLOB_ERR was set or (*errfunc)() returned non-zero.

#define GLOB_NOMATCH 2
// The pattern does not match any existing pathname, and GLOB_NOCHECK was not set in flags.

#define GLOB_NOSPACE 3
// An attempt to allocate memory failed.

int  glob(const char *, int, int(*)(const char *, int),
          glob_t *);
void globfree(glob_t *);

#endif
