/*
 * Somewhat reasonable way to disable warnings without generating
 * "unknown pragma" warnings in other compilers.
 * */

#ifndef DISABLE_WARNING_H
#define DISABLE_WARNING_H

#if defined(__GNUC__) || defined(__clang__)
    #define DO_PRAGMA(X) _Pragma(#X)
    #define DISABLE_WARNING_PUSH    DO_PRAGMA(GCC diagnostic push)
    #define DISABLE_WARNING_POP	    DO_PRAGMA(GCC diagnostic pop)
    #define DISABLE_WARNING(w)	    DO_PRAGMA(GCC diagnostic ignored #w)

    #define DISABLE_WARNING_SHADOW  DISABLE_WARNING(-Wshadow)
#else
    #define DISABLE_WARNING_PUSH
    #define DISABLE_WARNING_POP
    #define DISABLE_WARNING(w)

    #define DISABLE_WARNING_SHADOW
#endif

#endif
