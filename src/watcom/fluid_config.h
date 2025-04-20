/* hand-edited config for open-watcom based on fluid_config.cmake */

/* Activate debugging message */
#define DEBUG 0

/* Version number of the package */
#define VERSION "1.2.2"

/* Defined to 1 if the processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

#define SF3_DISABLED 0
#define SF3_XIPH_VORBIS 1
#define SF3_STB_VORBIS 2

#ifndef SF3_SUPPORT
#define SF3_SUPPORT SF3_DISABLED
#endif

/* if defined, the synth uses float (32 bit) samples, otherwise it uses double (64 bit) samples */
#define WITH_FLOAT

/* Standard C99 headers detection */ 
#define HAVE_STRING_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDARG_H 1
#define HAVE_MATH_H 1
#define HAVE_LIMITS_H 1
#define HAVE_FCNTL_H 1

//#pragma warning(disable : 4244)
//#pragma warning(disable : 4101)
//#pragma warning(disable : 4305)
//#pragma warning(disable : 4996)
