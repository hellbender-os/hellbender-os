#ifndef _INTTYPES_H
#define _INTTYPES_H

#include <stddef.h> // nonconforming: for wchar_t

// The Open Group Base Specifications Issue 7

#include <stdint.h>

typedef struct {
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;
//Structure type that is the type of the value returned by the imaxdiv() function.

// wchar_t As described in <stddef.h>.

/*
For each type that the implementation provides in <stdint.h>, the corresponding fprintf() and fwprintf() macros shall be defined and the corresponding fscanf() and fwscanf() macros shall be defined unless the implementation does not have a suitable modifier for the type.

In these names, N represents the width of the type as described in <stdint.h>. For example, PRIdFAST32 can be used in a format string to print the value of an integer of type int_fast32_t.

PRIdN

PRIdLEASTN

PRIdFASTN

PRIdMAX

PRIdPTR

PRIiN

PRIiLEASTN

PRIiFASTN

PRIiMAX

PRIiPTR

The fprintf() macros for unsigned integers are:

PRIoN

PRIoLEASTN

PRIoFASTN

PRIoMAX

PRIoPTR

PRIuN

PRIuLEASTN

PRIuFASTN

PRIuMAX

PRIuPTR

PRIxN

PRIxLEASTN

PRIxFASTN

PRIxMAX

PRIxPTR

PRIXN

PRIXLEASTN

PRIXFASTN

PRIXMAX

PRIXPTR

The fscanf() macros for signed integers are:

SCNdN

SCNdLEASTN

SCNdFASTN

SCNdMAX

SCNdPTR

SCNiN

SCNiLEASTN

SCNiFASTN

SCNiMAX

SCNiPTR

The fscanf() macros for unsigned integers are:

SCNoN

SCNoLEASTN

SCNoFASTN

SCNoMAX

SCNoPTR

SCNuN

SCNuLEASTN

SCNuFASTN

SCNuMAX

SCNuPTR

SCNxN

SCNxLEASTN

SCNxFASTN

SCNxMAX

SCNxPTR
*/

intmax_t  imaxabs(intmax_t);
imaxdiv_t imaxdiv(intmax_t, intmax_t);
intmax_t  strtoimax(const char *restrict, char **restrict, int);
uintmax_t strtoumax(const char *restrict, char **restrict, int);
intmax_t  wcstoimax(const wchar_t *restrict, wchar_t **restrict, int);
uintmax_t wcstoumax(const wchar_t *restrict, wchar_t **restrict, int);

#endif
