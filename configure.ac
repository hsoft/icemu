AC_PREREQ([2.69])
AC_INIT([icemu], [0.1])
AC_CONFIG_SRCDIR([src/pinlist.c])
AC_USE_SYSTEM_EXTENSIONS

AC_ARG_WITH(
    [ftdi],
    [AS_HELP_STRING(
        [--with-ftdi],
        [Build with libftdi]
    )],
    [],
    [with_ftdi=no]
)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([NCURSES], [ncurses])

if test "x$with_ftdi" != "xno"; then
    if test "x$with_ftdi" = "xyes"; then
        with_ftdi=libftdi
    fi
    PKG_CHECK_MODULES([FTDI], [$with_ftdi])
fi

PKG_CHECK_MODULES(
    [CUNIT],
    [cunit],
    [AC_DEFINE([HAVE_CUNIT])],
    []
)

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
# commenting AC_FUNC_STRNLEN out because it generated an error about missing
# install-sh script for some reason I don't know
#AC_FUNC_STRNLEN
AC_CHECK_FUNCS([gettimeofday memset])

AC_CONFIG_FILES([Makefile examples/common.mk])
AC_OUTPUT
