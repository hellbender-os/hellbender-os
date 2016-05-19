/* Useful if you wish to make target-specific gcc changes. */
#undef TARGET_HELLBENDER
#define TARGET_HELLBENDER 1

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtendS.o%s crtnS.o%s"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt0S.o%s crtiS.o%s crtbeginS.o%s"

#undef CC1_SPEC
#define CC1_SPEC "%{fPIC|fpic|shared|pie: -fvisibility=hidden}"

/* Default arguments you want when running your
   i686-hellbender-gcc/x86_64-hellbender-gcc toolchain */
#define LIB_SPEC "--start-group %{fPIC|fpic|shared|pie: %{!g:-lcS -lmS; :-lcSG -lmSG}; :%{!g:-lc -lm; :-lcG -lmG}} --end-group"

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
