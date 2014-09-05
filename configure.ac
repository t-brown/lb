#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ([2.63])
AC_INIT([lb],[0.1],[Timothy.Brown-1@colorado.edu])

# Define our build aux directory
AC_CONFIG_AUX_DIR([build-aux])

# Define our M4 macro directory
AC_CONFIG_MACRO_DIR([m4])

# Initialise automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Create the VERSION file, which contains the package version from
# AC_INIT.
echo AC_PACKAGE_VERSION > VERSION
AC_SUBST(PACKAGE_VERSION)

# Figure out what C compiler to use
#AC_PROG_CC([icc xlc pgcc pathcc craycc suncc gcc])
AC_ARG_WITH(mpi,
	[AS_HELP_STRING([--with-mpi],
		[compile with MPI (parallelization) support. If none is found,
		 MPI is not used. Default: yes])
	],
	[],
	[with_mpi=yes])
#
AX_PROG_CC_MPI([test x"$with_mpi" != xno],
	[use_mpi=yes],
	[use_mpi=no
	 if test x"$with_mpi" = xyes; then
		AC_MSG_FAILURE([MPI compiler requested, but couldn't use MPI.])
	 else
		AC_MSG_WARN([No MPI compiler found, won't use MPI.])
	 fi
])

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for functions and libraries
AC_CHECK_FUNCS([memset getprogname program_invocation_short_name])
AC_FUNC_MALLOC

# Output header and files
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile
		 src/Makefile
		 ])

AC_OUTPUT
echo
echo
echo "========================================================================"
echo "$PACKAGE_NAME              version $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "Installation prefix        : $prefix"
echo "C command                  : $CC $CFLAGS"
echo "Linker                     : $LD $LDFLAGS $LIBS"
echo
