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

//For each type that the implementation provides in <stdint.h>, the corresponding fprintf() and fwprintf() macros shall be defined and the corresponding fscanf() and fwscanf() macros shall be defined unless the implementation does not have a suitable modifier for the type.

//In these names, N represents the width of the type as described in <stdint.h>. For example, PRIdFAST32 can be used in a format string to print the value of an integer of type int_fast32_t.

#define __PREFIX_8__ "hh"
#define __PREFIX_16__ "h"
#define __PREFIX_32__ ""
#define __PREFIX_64__ "ll"
#define __PREFIX_LEAST64__ "ll"
#define __PREFIX_FAST64__ "ll"
#define __PREFIX_MAX__ "j"

#if (INT_LEAST8_MAX == INT8_MAX)
# define __PREFIX_LEAST8__ "hh"
#elif (INT_LEAST8_MAX == INT16_MAX)
# define __PREFIX_LEAST8__ "h"
#elif (INT_LEAST8_MAX == INT32_MAX)
# define __PREFIX_LEAST8__ ""
#elif (INT_LEAST8_MAX == INT64_MAX)
# define __PREFIX_LEAST8__ "ll"
#else
# error Cannot determine print type.
#endif

#if (INT_LEAST16_MAX == INT16_MAX)
# define __PREFIX_LEAST16__ "h"
#elif (INT_LEAST16_MAX == INT32_MAX)
# define __PREFIX_LEAST16__ ""
#elif (INT_LEAST16_MAX == INT64_MAX)
# define __PREFIX_LEAST16__ "ll"
#else
# error Cannot determine print type.
#endif

#if (INT_LEAST32_MAX == INT32_MAX)
# define __PREFIX_LEAST32__ ""
#elif (INT_LEAST32_MAX == INT64_MAX)
# define __PREFIX_LEAST32__ "ll"
#else
# error Cannot determine print type.
#endif


#if (INT_FAST8_MAX == INT8_MAX)
# define __PREFIX_FAST8__ "hh"
#elif (INT_FAST8_MAX == INT16_MAX)
# define __PREFIX_FAST8__ "h"
#elif (INT_FAST8_MAX == INT32_MAX)
# define __PREFIX_FAST8__ ""
#elif (INT_FAST8_MAX == INT64_MAX)
# define __PREFIX_FAST8__ "ll"
#else
# error Cannot determine print type.
#endif

#if (INT_FAST16_MAX == INT16_MAX)
# define __PREFIX_FAST16__ "h"
#elif (INT_FAST16_MAX == INT32_MAX)
# define __PREFIX_FAST16__ ""
#elif (INT_FAST16_MAX == INT64_MAX)
# define __PREFIX_FAST16__ "ll"
#else
# error Cannot determine print type.
#endif

#if (INT_FAST32_MAX == INT32_MAX)
# define __PREFIX_FAST32__ ""
#elif (INT_FAST32_MAX == INT64_MAX)
# define __PREFIX_FAST32__ "ll"
#else
# error Cannot determine print type.
#endif

#if (INTPTR_MAX == INT32_MAX)
# define __PREFIX_PTR__ ""
#elif (INTPTR_MAX == INT64_MAX)
# define __PREFIX_PTR__ "ll"
#else
# error Cannot determine print type.
#endif


#define PRId8 __PREFIX_8__ "d"
#define PRIi8 __PREFIX_8__ "i"
#define PRIo8 __PREFIX_8__ "o"
#define PRIu8 __PREFIX_8__ "u"
#define PRIx8 __PREFIX_8__ "x"
#define PRIX8 __PREFIX_8__ "X"

#define PRId16 __PREFIX_16__ "d"
#define PRIi16 __PREFIX_16__ "i"
#define PRIo16 __PREFIX_16__ "o"
#define PRIu16 __PREFIX_16__ "u"
#define PRIx16 __PREFIX_16__ "x"
#define PRIX16 __PREFIX_16__ "X"

#define PRId32 __PREFIX_32__ "d"
#define PRIi32 __PREFIX_32__ "i"
#define PRIo32 __PREFIX_32__ "o"
#define PRIu32 __PREFIX_32__ "u"
#define PRIx32 __PREFIX_32__ "x"
#define PRIX32 __PREFIX_32__ "X"

#define PRId64 __PREFIX_64__ "d"
#define PRIi64 __PREFIX_64__ "i"
#define PRIo64 __PREFIX_64__ "o"
#define PRIu64 __PREFIX_64__ "u"
#define PRIx64 __PREFIX_64__ "x"
#define PRIX64 __PREFIX_64__ "X"

#define PRIdLEAST8 __PREFIX_LEAST8__ "d"
#define PRIiLEAST8 __PREFIX_LEAST8__ "i"
#define PRIoLEAST8 __PREFIX_LEAST8__ "o"
#define PRIuLEAST8 __PREFIX_LEAST8__ "u"
#define PRIxLEAST8 __PREFIX_LEAST8__ "x"
#define PRIXLEAST8 __PREFIX_LEAST8__ "X"

#define PRIdLEAST16 __PREFIX_LEAST16__ "d"
#define PRIiLEAST16 __PREFIX_LEAST16__ "i"
#define PRIoLEAST16 __PREFIX_LEAST16__ "o"
#define PRIuLEAST16 __PREFIX_LEAST16__ "u"
#define PRIxLEAST16 __PREFIX_LEAST16__ "x"
#define PRIXLEAST16 __PREFIX_LEAST16__ "X"

#define PRIdLEAST32 __PREFIX_LEAST32__ "d"
#define PRIiLEAST32 __PREFIX_LEAST32__ "i"
#define PRIoLEAST32 __PREFIX_LEAST32__ "o"
#define PRIuLEAST32 __PREFIX_LEAST32__ "u"
#define PRIxLEAST32 __PREFIX_LEAST32__ "x"
#define PRIXLEAST32 __PREFIX_LEAST32__ "X"

#define PRIdLEAST64 __PREFIX_LEAST64__ "d"
#define PRIiLEAST64 __PREFIX_LEAST64__ "i"
#define PRIoLEAST64 __PREFIX_LEAST64__ "o"
#define PRIuLEAST64 __PREFIX_LEAST64__ "u"
#define PRIxLEAST64 __PREFIX_LEAST64__ "x"
#define PRIXLEAST64 __PREFIX_LEAST64__ "X"

#define PRIdFAST8 __PREFIX_FAST8__ "d"
#define PRIiFAST8 __PREFIX_FAST8__ "i"
#define PRIoFAST8 __PREFIX_FAST8__ "o"
#define PRIuFAST8 __PREFIX_FAST8__ "u"
#define PRIxFAST8 __PREFIX_FAST8__ "x"
#define PRIXFAST8 __PREFIX_FAST8__ "X"

#define PRIdFAST16 __PREFIX_FAST16__ "d"
#define PRIiFAST16 __PREFIX_FAST16__ "i"
#define PRIoFAST16 __PREFIX_FAST16__ "o"
#define PRIuFAST16 __PREFIX_FAST16__ "u"
#define PRIxFAST16 __PREFIX_FAST16__ "x"
#define PRIXFAST16 __PREFIX_FAST16__ "X"

#define PRIdFAST32 __PREFIX_FAST32__ "d"
#define PRIiFAST32 __PREFIX_FAST32__ "i"
#define PRIoFAST32 __PREFIX_FAST32__ "o"
#define PRIuFAST32 __PREFIX_FAST32__ "u"
#define PRIxFAST32 __PREFIX_FAST32__ "x"
#define PRIXFAST32 __PREFIX_FAST32__ "X"

#define PRIdFAST64 __PREFIX_FAST64__ "d"
#define PRIiFAST64 __PREFIX_FAST64__ "i"
#define PRIoFAST64 __PREFIX_FAST64__ "o"
#define PRIuFAST64 __PREFIX_FAST64__ "u"
#define PRIxFAST64 __PREFIX_FAST64__ "x"
#define PRIXFAST64 __PREFIX_FAST64__ "X"

#define PRIdMAX __PREFIX_MAX__ "d"
#define PRIiMAX __PREFIX_MAX__ "i"
#define PRIoMAX __PREFIX_MAX__ "o"
#define PRIuMAX __PREFIX_MAX__ "u"
#define PRIxMAX __PREFIX_MAX__ "x"
#define PRIXMAX __PREFIX_MAX__ "X"

#define PRIdPTR __PREFIX_PTR__ "d"
#define PRIiPTR __PREFIX_PTR__ "i"
#define PRIoPTR __PREFIX_PTR__ "o"
#define PRIuPTR __PREFIX_PTR__ "u"
#define PRIxPTR __PREFIX_PTR__ "x"
#define PRIXPTR __PREFIX_PTR__ "X"

// The fscanf() macros

#define SCNd8 __PREFIX_8__ "d"
#define SCNi8 __PREFIX_8__ "i"
#define SCNo8 __PREFIX_8__ "o"
#define SCNu8 __PREFIX_8__ "u"
#define SCNx8 __PREFIX_8__ "x"

#define SCNd16 __PREFIX_16__ "d"
#define SCNi16 __PREFIX_16__ "i"
#define SCNo16 __PREFIX_16__ "o"
#define SCNu16 __PREFIX_16__ "u"
#define SCNx16 __PREFIX_16__ "x"

#define SCNd32 __PREFIX_32__ "d"
#define SCNi32 __PREFIX_32__ "i"
#define SCNo32 __PREFIX_32__ "o"
#define SCNu32 __PREFIX_32__ "u"
#define SCNx32 __PREFIX_32__ "x"

#define SCNd64 __PREFIX_64__ "d"
#define SCNi64 __PREFIX_64__ "i"
#define SCNo64 __PREFIX_64__ "o"
#define SCNu64 __PREFIX_64__ "u"
#define SCNx64 __PREFIX_64__ "x"

#define SCNdLEAST8 __PREFIX_LEAST8__ "d"
#define SCNiLEAST8 __PREFIX_LEAST8__ "i"
#define SCNoLEAST8 __PREFIX_LEAST8__ "o"
#define SCNuLEAST8 __PREFIX_LEAST8__ "u"
#define SCNxLEAST8 __PREFIX_LEAST8__ "x"

#define SCNdLEAST16 __PREFIX_LEAST16__ "d"
#define SCNiLEAST16 __PREFIX_LEAST16__ "i"
#define SCNoLEAST16 __PREFIX_LEAST16__ "o"
#define SCNuLEAST16 __PREFIX_LEAST16__ "u"
#define SCNxLEAST16 __PREFIX_LEAST16__ "x"

#define SCNdLEAST32 __PREFIX_LEAST32__ "d"
#define SCNiLEAST32 __PREFIX_LEAST32__ "i"
#define SCNoLEAST32 __PREFIX_LEAST32__ "o"
#define SCNuLEAST32 __PREFIX_LEAST32__ "u"
#define SCNxLEAST32 __PREFIX_LEAST32__ "x"

#define SCNdLEAST64 __PREFIX_LEAST64__ "d"
#define SCNiLEAST64 __PREFIX_LEAST64__ "i"
#define SCNoLEAST64 __PREFIX_LEAST64__ "o"
#define SCNuLEAST64 __PREFIX_LEAST64__ "u"
#define SCNxLEAST64 __PREFIX_LEAST64__ "x"

#define SCNdFAST8 __PREFIX_FAST8__ "d"
#define SCNiFAST8 __PREFIX_FAST8__ "i"
#define SCNoFAST8 __PREFIX_FAST8__ "o"
#define SCNuFAST8 __PREFIX_FAST8__ "u"
#define SCNxFAST8 __PREFIX_FAST8__ "x"

#define SCNdFAST16 __PREFIX_FAST16__ "d"
#define SCNiFAST16 __PREFIX_FAST16__ "i"
#define SCNoFAST16 __PREFIX_FAST16__ "o"
#define SCNuFAST16 __PREFIX_FAST16__ "u"
#define SCNxFAST16 __PREFIX_FAST16__ "x"

#define SCNdFAST32 __PREFIX_FAST32__ "d"
#define SCNiFAST32 __PREFIX_FAST32__ "i"
#define SCNoFAST32 __PREFIX_FAST32__ "o"
#define SCNuFAST32 __PREFIX_FAST32__ "u"
#define SCNxFAST32 __PREFIX_FAST32__ "x"

#define SCNdFAST64 __PREFIX_FAST64__ "d"
#define SCNiFAST64 __PREFIX_FAST64__ "i"
#define SCNoFAST64 __PREFIX_FAST64__ "o"
#define SCNuFAST64 __PREFIX_FAST64__ "u"
#define SCNxFAST64 __PREFIX_FAST64__ "x"

#define SCNdMAX __PREFIX_MAX__ "d"
#define SCNiMAX __PREFIX_MAX__ "i"
#define SCNoMAX __PREFIX_MAX__ "o"
#define SCNuMAX __PREFIX_MAX__ "u"
#define SCNxMAX __PREFIX_MAX__ "x"

#define SCNdPTR __PREFIX_PTR__ "d"
#define SCNiPTR __PREFIX_PTR__ "i"
#define SCNoPTR __PREFIX_PTR__ "o"
#define SCNuPTR __PREFIX_PTR__ "u"
#define SCNxPTR __PREFIX_PTR__ "x"

intmax_t  imaxabs(intmax_t);
imaxdiv_t imaxdiv(intmax_t, intmax_t);
intmax_t  strtoimax(const char *, char **, int);
uintmax_t strtoumax(const char *, char **, int);
intmax_t  wcstoimax(const wchar_t *, wchar_t **, int);
uintmax_t wcstoumax(const wchar_t *, wchar_t **, int);

#endif
