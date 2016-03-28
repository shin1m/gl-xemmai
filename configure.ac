AC_INIT([gl-xemmai], [201603])
AM_INIT_AUTOMAKE([foreign nostdinc dist-bzip2 no-dist-gzip])
AC_CONFIG_SRCDIR([gl/xemmai/gl.cc])

PKG_CHECK_MODULES([GL], [gl >= 7.11])
XEMMAI_USE=xemmai-201603
PKG_CHECK_MODULES([XEMMAI], [${XEMMAI_USE}], [
	XEMMAI_MODULE_PATH=`pkg-config --variable=module_path ${XEMMAI_USE}`
])
AC_SUBST(XEMMAI_MODULE_PATH)

if test "${CXXFLAGS+set}" != set; then
	CXXFLAGS=
fi
AC_PROG_CXX
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

AC_ARG_ENABLE(
	[debug],
	AS_HELP_STRING([--enable-debug], [turn on debugging]),
	[case "${enableval}" in
	  yes) debug=true ;;
	  no) debug=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	 esac],
	[debug=false]
)
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AC_ARG_ENABLE(
	[profile],
	AS_HELP_STRING([--enable-profile], [turn on profiling]),
	[case "${enableval}" in
	  yes) profile=true ;;
	  no) profile=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-profile]) ;;
	 esac],
	[profile=false]
)
AM_CONDITIONAL([PROFILE], [test x$profile = xtrue])

AC_CONFIG_HEADERS([gl/xemmai/configure.h])
AC_CONFIG_FILES([
	Makefile
	gl/xemmai/Makefile
])
AC_OUTPUT
