/* $FreeBSD$ */

#include <osreldate.h>

/* config.h.  Generated by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
/* Autoheader needs me */
#define PACKAGE "file"

/* Autoheader needs me */
#define VERSION "4.12"

/* Define if builtin ELF support is enabled.  */
#define BUILTIN_ELF 1

/* Define if ELF core file support is enabled.  */
#define ELFCORE 1

/* Define if the `long long' type works.  */
#define HAVE_LONG_LONG 1

/* Define if we have "tm_zone" in "struct tm".  */
#define HAVE_TM_ZONE 1

/* Define if we have a global "char * []" "tzname" variable.  */
#define HAVE_TZNAME 1

/* Define if we have "tm_isdst" in "struct tm".  */
#define HAVE_TM_ISDST 1

/* Define if we have a global "int" variable "daylight".  */
/* #undef HAVE_DAYLIGHT */

/* Define if we have a mkstemp */
#define HAVE_MKSTEMP 1

/* Define to `unsigned char' if standard headers don't define.  */
/* #undef uint8_t */

/* Define to `unsigned short' if standard headers don't define.  */
/* #undef uint16_t */

/* Define to `unsigned int' if standard headers don't define.  */
/* #undef uint32_t */

/* Define to `unsigned long long', if available, or `unsigned long', if
   standard headers don't define.  */
/* #undef uint64_t */

/* Define to `int' if standard headers don't define.  */
/* #undef int32_t */

/* FIXME: These have to be added manually because autoheader doesn't know
   about AC_CHECK_SIZEOF_INCLUDES.  */

/* The number of bytes in a uint8_t.  */
#define SIZEOF_UINT8_T 1

/* The number of bytes in a uint16_t.  */
#define SIZEOF_UINT16_T 2

/* The number of bytes in a uint32_t.  */
#define SIZEOF_UINT32_T 4

/* The number of bytes in a uint64_t.  */
#define SIZEOF_UINT64_T 8

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the `mbrtowc' function. */
#define HAVE_MBRTOWC 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the `mmap' function. */
#define HAVE_MMAP 1

/* Define to 1 if you have the <stdint.h> header file. */
#if __FreeBSD_version >= 500019
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if `st_rdev' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_RDEV 1

/* Define to 1 if your `struct stat' has `st_rdev'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_RDEV' instead. */
#define HAVE_ST_RDEV 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/utime.h> header file. */
/* #undef HAVE_SYS_UTIME_H */

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* HAVE_TM_ZONE */
#define HAVE_TM_ZONE 1

/* HAVE_TZNAME */
#define HAVE_TZNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `utime' function. */
#define HAVE_UTIME 1

/* Define to 1 if you have the `utimes' function. */
#define HAVE_UTIMES 1

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if you have the `wcwidth' function. */
#define HAVE_WCWIDTH 1

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
   */
/* #undef MAJOR_IN_MKDEV */

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
/* #undef MAJOR_IN_SYSMACROS */

/* Name of package */
#define PACKAGE "file"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Version number of package */
#define VERSION "4.12"

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to a type if <wchar.h> does not define. */
/* #undef mbstate_t */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */
