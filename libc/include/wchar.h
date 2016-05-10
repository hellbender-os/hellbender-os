#ifndef _WCHAR_H
#define _WCHAR_H

#include <locale.h> // nonconforming: locale_t
#include <stdint.h> // nonconforming: WCHAR_MAX, WCHAR_MIN

// The Open Group Base Specifications Issue 7

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _mbstate {
} mbstate_t;
//An object type other than an array type that can hold the conversion state information necessary to convert between sequences of (possibly multi-byte) characters and wide characters. [CX] [Option Start]  If a codeset is being used such that an mbstate_t needs to preserve more than two levels of reserved state, the results are unspecified. [Option End]

typedef int wctype_t;
//[OB XSI] [Option Start] A scalar type of a data object that can hold values which represent locale-specific character classification. [Option End]

typedef int wint_t;
//An integer type capable of storing any valid value of wchar_t or WEOF.

struct tm;
// a tag naming an incomplete structure type, the contents of which are described in the <time.h> header.

#define WEOF -1
//Constant expression of type wint_t that is returned by several WP functions to indicate end-of-file.

wint_t        btowc(int);
wint_t        fgetwc(FILE *);
wchar_t      *fgetws(wchar_t *, int, FILE *);
wint_t        fputwc(wchar_t, FILE *);
int           fputws(const wchar_t *, FILE *);
int           fwide(FILE *, int);
int           fwprintf(FILE *, const wchar_t *, ...);
int           fwscanf(FILE *, const wchar_t *, ...);
wint_t        getwc(FILE *);
wint_t        getwchar(void);
int           iswalnum(wint_t);
int           iswalpha(wint_t);
int           iswcntrl(wint_t);
int           iswctype(wint_t, wctype_t);
int           iswdigit(wint_t);
int           iswgraph(wint_t);
int           iswlower(wint_t);
int           iswprint(wint_t);
int           iswpunct(wint_t);
int           iswspace(wint_t);
int           iswupper(wint_t);
int           iswxdigit(wint_t);
size_t        mbrlen(const char *, size_t, mbstate_t *);
size_t        mbrtowc(wchar_t *, const char *, size_t,
                      mbstate_t *);
int           mbsinit(const mbstate_t *);
size_t        mbsnrtowcs(wchar_t *, const char **,
                         size_t, size_t, mbstate_t *);
size_t        mbsrtowcs(wchar_t *, const char **, size_t,
                        mbstate_t *);
FILE         *open_wmemstream(wchar_t **, size_t *);
wint_t        putwc(wchar_t, FILE *);
wint_t        putwchar(wchar_t);
int           swprintf(wchar_t *, size_t,
                       const wchar_t *, ...);
int           swscanf(const wchar_t *,
                      const wchar_t *, ...);
wint_t        towlower(wint_t);
wint_t        towupper(wint_t);
wint_t        ungetwc(wint_t, FILE *);
int           vfwprintf(FILE *, const wchar_t *, va_list);
int           vfwscanf(FILE *, const wchar_t *, va_list);
int           vswprintf(wchar_t *, size_t,
                        const wchar_t *, va_list);
int           vswscanf(const wchar_t *, const wchar_t *,
                       va_list);
int           vwprintf(const wchar_t *, va_list);
int           vwscanf(const wchar_t *, va_list);
wchar_t      *wcpcpy(wchar_t *, const wchar_t *);
wchar_t      *wcpncpy(wchar_t *, const wchar_t *, size_t);
size_t        wcrtomb(char *, wchar_t, mbstate_t *);
int           wcscasecmp(const wchar_t *, const wchar_t *);
int           wcscasecmp_l(const wchar_t *, const wchar_t *, locale_t);
wchar_t      *wcscat(wchar_t *, const wchar_t *);
wchar_t      *wcschr(const wchar_t *, wchar_t);
int           wcscmp(const wchar_t *, const wchar_t *);
int           wcscoll(const wchar_t *, const wchar_t *);
int           wcscoll_l(const wchar_t *, const wchar_t *, locale_t);
wchar_t      *wcscpy(wchar_t *, const wchar_t *);
size_t        wcscspn(const wchar_t *, const wchar_t *);
wchar_t      *wcsdup(const wchar_t *);
size_t        wcsftime(wchar_t *, size_t,
                       const wchar_t *, const struct tm *);
size_t        wcslen(const wchar_t *);
int           wcsncasecmp(const wchar_t *, const wchar_t *, size_t);
int           wcsncasecmp_l(const wchar_t *, const wchar_t *, size_t,
                            locale_t);
wchar_t      *wcsncat(wchar_t *, const wchar_t *, size_t);
int           wcsncmp(const wchar_t *, const wchar_t *, size_t);
wchar_t      *wcsncpy(wchar_t *, const wchar_t *, size_t);
size_t        wcsnlen(const wchar_t *, size_t);
size_t        wcsnrtombs(char *, const wchar_t **, size_t,
                         size_t, mbstate_t *);
wchar_t      *wcspbrk(const wchar_t *, const wchar_t *);
wchar_t      *wcsrchr(const wchar_t *, wchar_t);
size_t        wcsrtombs(char *, const wchar_t **,
                        size_t, mbstate_t *);
size_t        wcsspn(const wchar_t *, const wchar_t *);
wchar_t      *wcsstr(const wchar_t *, const wchar_t *);
double        wcstod(const wchar_t *, wchar_t **);
float         wcstof(const wchar_t *, wchar_t **);
wchar_t      *wcstok(wchar_t *, const wchar_t *,
                     wchar_t **);
long          wcstol(const wchar_t *, wchar_t **, int);
long double   wcstold(const wchar_t *, wchar_t **);
long long     wcstoll(const wchar_t *, wchar_t **, int);
unsigned long wcstoul(const wchar_t *, wchar_t **, int);
unsigned long long
              wcstoull(const wchar_t *, wchar_t **, int);
int           wcswidth(const wchar_t *, size_t);
size_t        wcsxfrm(wchar_t *, const wchar_t *, size_t);
size_t        wcsxfrm_l(wchar_t *, const wchar_t *,
                        size_t, locale_t);
int           wctob(wint_t);
wctype_t      wctype(const char *);
int           wcwidth(wchar_t);
wchar_t      *wmemchr(const wchar_t *, wchar_t, size_t);
int           wmemcmp(const wchar_t *, const wchar_t *, size_t);
wchar_t      *wmemcpy(wchar_t *, const wchar_t *, size_t);
wchar_t      *wmemmove(wchar_t *, const wchar_t *, size_t);
wchar_t      *wmemset(wchar_t *, wchar_t, size_t);
int           wprintf(const wchar_t *, ...);
int           wscanf(const wchar_t *, ...);

#endif
