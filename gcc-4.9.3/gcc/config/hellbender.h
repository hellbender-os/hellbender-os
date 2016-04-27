/* Useful if you wish to make target-specific gcc changes. */
#undef TARGET_HELLBENDER
#define TARGET_HELLBENDER 1

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtendS.o%s crtn.o%s"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crti.o%s crtbeginS.o%s"

#undef CC1_SPEC
#define CC1_SPEC "%{!ffreestanding:-fPIC -fvisibility=hidden}"

/* Default arguments you want when running your
   i686-hellbender-gcc/x86_64-hellbender-gcc toolchain */
#define LIB_SPEC "-lc -lg -lm" /* link against C standard libraries */
                               /* modify this based on your needs */

/* Don't automatically add extern "C" { } around header files. */
#undef  NO_IMPLICIT_EXTERN_C
#define NO_IMPLICIT_EXTERN_C 1
 
/* Additional predefined macros. */
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()      \
  do {                                \
    builtin_define ("__hellbender__");      \
    builtin_define ("__unix__");      \
    builtin_assert ("system=hellbender");   \
    builtin_assert ("system=unix");   \
    builtin_assert ("system=posix");   \
  } while(0);
