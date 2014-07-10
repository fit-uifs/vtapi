#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([VTApi (Video Terror API)], [2.0], [chmelarp@fit.vutbr.cz], [vtapi], [http://vidte.fit.vutbr.cz])
AM_INIT_AUTOMAKE

AC_PREREQ([2.68]) # vyzaduje alespon autoconf ver. 2.6.8

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Use entered compiler for checks
AC_LANG([C++])

# Checks for libraries.

# aby se dalo vyrobit .so
LT_INIT([disable-static])
AC_LIBTOOL_DLOPEN
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL(libtool)

AC_SUBST([VTAPI_SO_VERSION], [2:0:1])
AC_SUBST([VTAPI_API_VERSION], [2.0])


CPPFLAGS="$CPPFLAGS $VTAPI_CPPFLAGS"
LDFLAGS="$LDFLAGS $VTAPI_LDFLAGS"


# ltdl (yum install libtool-ltdl-devel)
# -lltdl
AC_CHECK_HEADER(ltdl.h, [], [AC_MSG_ERROR([the ltdl.h header is missing. Please install package libtool-ltdl-devel!])])
LTDL_CFLAGS=""
LTDL_LIBS="-lltdl"
AC_SUBST([LTDL_CFLAGS])
AC_SUBST([LTDL_LIBS])


# readproc (yum install procps-ng-devel)
readproc_present="yes"
AC_CHECK_HEADER(proc/readproc.h, [], [readproc_present="no"])
AC_CHECK_LIB([procps], [readproctab], [READPROC_LIBS="-lprocps"])
AC_CHECK_LIB([proc], [readproctab], [READPROC_LIBS="-lproc"])

if test -z "$READPROC_LIBS"
then
  readproc_present="no"
fi

if test x"$readproc_present" == "xyes"
then
  AC_DEFINE([HAVE_READPROC], [1], [readproc is present])
  
  READPROC_CFLAGS=""
  
  AC_SUBST([READPROC_CFLAGS])
  AC_SUBST([READPROC_LIBS])
fi

AC_PATH_PROG([PKG_CONFIG], [pkg-config], [AC_MSG_ERROR([pkg-config not available!])])

# opencv 
opencv_present="yes"
# PKG_CHECK_MODULES([opencv], [opencv >= 2.4.1], [], [opencv_present="no"])

opencv_MODVERSION=`"$PKG_CONFIG" opencv --modversion` 
if test -z "$opencv_MODVERSION"
then
  opencv_present="no"
else 
  opencv_CFLAGS=`"$PKG_CONFIG" opencv --cflags`
  opencv_LIBS=`"$PKG_CONFIG" opencv --libs`
  AC_SUBST([opencv_CFLAGS])
  AC_SUBST([opencv_LIBS])  
fi

AC_ARG_WITH([opencv], AS_HELP_STRING([--without-opencv], [compile without opencv support]))

opencv_want=yes
AS_IF([test "x$with_opencv" != "xno"],
      [],
      [opencv_want=no])


if test "x$opencv_want" != "xno" -a x"$opencv_present" == "xno"
then
  AC_MSG_ERROR([Error, FIXME prelozit - bud nainstaluj opencv nebo zadej --without-opencv])
fi

if test x"$opencv_want" != "xno"
then
  AC_DEFINE([HAVE_OPENCV], [1], [opencv is present])
fi

# sqlite
sqlite_present="yes"
#PKG_CHECK_MODULES([sqlite3], [sqlite3 >= 3.7.0], [],[sqlite_present="no"])
sqlite3_MODVERSION=`"$PKG_CONFIG" sqlite3 --modversion` 
if test -z "$sqlite3_MODVERSION"
then
  sqlite_present="no"
else 
  sqlite3_CFLAGS=`"$PKG_CONFIG" sqlite3 --cflags`
  sqlite3_LIBS=`"$PKG_CONFIG" sqlite3 --libs`
  AC_SUBST([sqlite3_CFLAGS])
  AC_SUBST([sqlite3_LIBS])  
fi


AC_ARG_WITH([sqlite], AS_HELP_STRING([--without-sqlite], [compile without sqlite3 support i.e. force postgresql]))
AS_IF([test "x$with_sqlite" != "xno"], [], [sqlite_present=no])

if test x"$sqlite_present" == "xyes" 
then
  AC_DEFINE([HAVE_SQLITE], [1], [SQLite is present])
fi

# liblwgeom #-llwgeom

lwgeom_present="yes"
AC_CHECK_HEADER(liblwgeom.h, [], [lwgeom_present="no"])

#AC_ARG_WITH([liblwgeom-include-path],
#  [AS_HELP_STRING([--with-liblwgeom-include-path],
#    [location of the liblwgeom headers, defaults to /usr/local/include/lwgeom])],
#  [
#   LIBLWGEOM_CFLAGS="-I$withval"
#   AC_CHECK_HEADER($withval/liblwgeom.h, [], [liblwgeom_present=0])
#  ],
#  [
#   LIBLWGEOM_CFLAGS='-I/usr/local/include/lwgeom'
#   AC_CHECK_HEADER(/usr/local/include/lwgeom/liblwgeom.h, [], [liblwgeom_present=0])
#  ])
#
#AC_SUBST([LIBLWGEOM_CFLAGS])
#
#
#AC_ARG_WITH([liblwgeom-lib-path],
#  [AS_HELP_STRING([--with-liblwgeom-lib-path], [location of the liblwgeom libraries])],
#  [
#   LIBLWGEOM_LIBS="-L$withval -llwgeom"
#  ],
#  [LIBLWGEOM_LIBS='-llwgeom -L/usr/local/lib'])
#AC_SUBST([LIBLWGEOM_LIBS])
#
#if test "$liblwgeom_present" == 0
#then
#    AC_MSG_ERROR([Error, the liblwgeom header is missing!])
#else 
#  AC_DEFINE([HAVE_LIBLWGEOM], [1], [liblwgeom is present])
#fi

# GEOS # -lgeos_c

geos_present="yes"
AC_PATH_PROG([GEOS_CONFIG], [geos-config],[false])
if test x"$GEOS_CONFIG" == "xfalse"
then
  geos_present="no"
  AC_MSG_WARN([geos not available!])
else
  GEOS_MAJOR=`$GEOS_CONFIG --version | cut -d. -f1`
  GEOS_MINOR=`$GEOS_CONFIG --version | cut -d. -f2`
  GEOS_VERNUM=`expr $GEOS_MAJOR "*" 10`
  GEOS_VERNUM=`expr $GEOS_VERNUM "+" $GEOS_MINOR`

  if test "$GEOS_VERNUM" -lt 32; then # starsi verze nez 3.2.0 tak je to asi problem
    AC_MSG_ERROR([geos should be at least 3.2.0 or newer])
  fi

  AC_MSG_CHECKING([for GEOS include directory])
  GEOS_CFLAGS=`"$GEOS_CONFIG" --cflags`
  AC_MSG_RESULT([$GEOS_CFLAGS])
  AC_SUBST([GEOS_CFLAGS])

  AC_MSG_CHECKING([for GEOS libs])
  GEOS_LIBS=`"$GEOS_CONFIG" --libs`
  AC_MSG_RESULT([$GEOS_LIBS])
  AC_SUBST([GEOS_LIBS])

  AC_DEFINE([HAVE_GEOS], [1], [geos is present])
fi

# FIXME 
# dalsi veci pro postgis
# pgDistance, cube, pgCubeBox3d

# postresql
postgresql_present="yes"

AC_PATH_PROG([PG_CONFIG], [pg_config],[false])
if test x"$PG_CONFIG" == "xfalse"
then
  postgresql_present="no"
else
  PQ_CFLAGS="-I$("$PG_CONFIG" --includedir)"
  PQ_LIBS="-L$("$PG_CONFIG" --libdir) -lpq -lpqtypes"
fi

TMP_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$PQ_CFLAGS ${CPPFLAGS}"
TMP_LIBS="$LIBS"
LIBS="$PQ_LIBS ${LIBS}"
AC_CHECK_HEADER(libpq-fe.h, [], [postgresql_present="no"]) 
AC_CHECK_HEADER(libpqtypes.h, [], [postgresql_present="no"])
# FIXME: some problems on OS Windows
AC_CHECK_LIB([pq], [PQserverVersion], [], [postgresql_present="no"])
AC_CHECK_LIB([pqtypes], [PQinitTypes], [], [postgresql_present="no"])
CPPFLAGS="$TMP_CPPFLAGS"
LIBS="$TMP_LIBS"

if test x"$postgresql_present" == "xno"
then
  AC_MSG_WARN([Postgresql is not available!])
else 
  AC_DEFINE([HAVE_POSTGRESQL], [1], [postgresql is present])
  AC_SUBST([PQ_CFLAGS])
  AC_SUBST([PQ_LIBS])
fi





# postgresql or sqlite support is necessary
if test "x$postgresql_present" = "xno" -a "x$sqlite_present" = "xno"
then
    AC_MSG_ERROR([Error, vtapi needs postgresql or sqlite])
fi


# all postgis requirements

AC_ARG_WITH([postgis], AS_HELP_STRING([--without-postgis], [compile without postgis support]))

postgis_want=yes
AS_IF([test "x$with_postgis" != "xno"],
      [],
      [postgis_want=no])

## TODO: small hack to avoid collapse of make due to postgis (!!!DEVEL: comment next line)
postgis_want="no"


if test "x$postgresql_present" = "xno" -o "x$geos_present" = "xno" -o "x$lwgeom_present" = "xno"
then
  postgis_supported="no"
else 
  postgis_supported="yes"
fi

if test "x$postgis_supported" = "xno" -a "x$postgis_want" != "xno" 
then
  AC_MSG_ERROR([Error, FIXME prelozit - bud nainstaluj vsecky veci k postgisu nebo zadej --without-postgis])  
fi

if test "x$postgis_want" != "xno" # pokud postgis neni zakazany, a neskoniclo to s AC_MSG_ERROR, tak ho asi mame (-;
then
  AC_DEFINE([HAVE_POSTGIS], [1], [geos, lwgeom, and postgres are available])
fi


# Checks for header files.

# --enable-doc
AC_ARG_ENABLE([doc],
    AS_HELP_STRING([--enable-doc], [Build the documentation]))

with_doc=no
AS_IF([test "x$enable_doc" = "xyes"], [
    with_doc=yes

    AC_CHECK_PROG(HAVE_DOXYGEN, doxygen, true)
    AC_CONFIG_FILES([doc/Makefile doc/Doxyfile])
])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$HAVE_DOXYGEN"]) # prenese do Makefile.am podminku - HAVE_DOXYGEN se testuje jen pokud je povoleny



# Checks for typedefs, structures, and compiler characteristics.
# AC_CHECK_HEADER_STDBOOL
# AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset select strchr strcspn strspn strtol])


# Platform-dependent variables
WIN32_EXTRA_LDFLAGS=
case "$host" in
 *-*-cygwin*)
   WIN32_EXTRA_LDFLAGS="-no-undefined"
 ;;
 *-*-mingw*)
   WIN32_EXTRA_LDFLAGS="-no-undefined"
 ;;
esac
AC_SUBST(WIN32_EXTRA_LDFLAGS)


AC_OUTPUT_COMMANDS([echo "

  OpenCV:              $opencv_present
  procps:              $readproc_present
  sqlite3:             $sqlite_present
  postgresql:          $postgresql_present
  postgis:             $postgis_supported 
   - geos:             $geos_present
   - lwgeom:           $lwgeom_present
   - cube:             FIXME 
   - other             FIXME
  documentation:       $doxygen_enabled

"],[opencv_present=$opencv_want
sqlite_present=$sqlite_present
readproc_present=$readproc_present
postgresql_present=$postgresql_present
geos_present=$geos_present
lwgeom_present=$lwgeom_present
postgis_supported=$postgis_want

doxygen_enabled=$with_doc])

AC_CONFIG_FILES([src/Makefile Makefile])

#AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile])
AC_OUTPUT
