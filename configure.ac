AC_PREREQ([2.69])
AC_INIT([lenc], [0.1], [fanyunfeng@sina.com])
#AC_CONFIG_SRCDIR([src/encoder.h])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.10 -Wall -Werror -Wno-override])

AM_PROG_AR

LT_INIT
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
C_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([malloc.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
        lib/Makefile
        regex/Makefile])


AC_OUTPUT
