#ifndef _STDDEF_H
#define _STDDEF_H

// The Open Group Base Specifications Issue 7

#define NULL ((void*)0)
//Null pointer constant. [CX] [Option Start]  The macro shall expand to an integer constant expression with the value 0 cast to type void *. [Option End]

#define offsetof(st, m) __builtin_offsetof(st, m)
//Integer constant expression of type size_t, the value of which is the offset in bytes to the structure member (member-designator), from the beginning of its structure (type).

typedef int ptrdiff_t;
//Signed integer type of the result of subtracting two pointers.

typedef unsigned wchar_t;
//Integer type whose range of values can represent distinct codes for all members of the largest extended character set specified among the supported locales; the null character shall have the code value zero. Each member of the basic character set shall have a code value equal to its value when used as the lone character in an integer character constant if an implementation does not define __STDC_MB_MIGHT_NEQ_WC__.

typedef __SIZE_TYPE__ size_t;
//Unsigned integer type of the result of the sizeof operator.

#endif
     
