#ifndef _ASSERT_H
#define _ASSERT_H

// The Open Group Base Specifications Issue 7

#undef assert
#ifdef NDEBUG
# define assert(ignore)((void) 0)
#else
void _assert(const char*)
# define assert(exp) if (!exp) _assert("Assertion failed at " __FILE__ ":" __LINE__ "; " #exp)
#endif

#endif
