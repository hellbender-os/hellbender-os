#ifndef _FNMATCH_H
#define _FNMATCH_H

// The Open Group Base Specifications Issue 7

#define FNM_NOMATCH 1
// The string does not match the specified pattern.

#define FNM_PATHNAME
// <slash> in string only matches <slash> in pattern.

#define FNM_PERIOD
// Leading <period> in string must be exactly matched by <period> in pattern.

#define FNM_NOESCAPE
// Disable backslash escaping.

int fnmatch(const char *, const char *, int);

#endif