#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script

AC_INIT([VTApi (Video Terror API)], [2.0], [ivolf@fit.vutbr.cz], [vtapi], [http://vidte.fit.vutbr.cz])
AM_INIT_AUTOMAKE

AC_PREREQ([2.63]) # originally, there was autoconf ver. 2.68, but it seems that 2.63 is also OK ;)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([include/common/vtapi_autoconfig.h])

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

VTAPI_API_VERSION=2.0
VTAPI_SO_VERSION=${VTAPI_API_VERSION//\./\:}
AC_SUBST([VTAPI_API_VERSION])
AC_SUBST([VTAPI_SO_VERSION])


CPPFLAGS="${CPPFLAGS} ${VTAPI_CPPFLAGS}"
LDFLAGS="${LDFLAGS} ${VTAPI_LDFLAGS}"


###############################################################################
#####   Additions to autoheader (vtapi_autoconfig.h.in) file              #####
###############################################################################
AH_TOP([/**
 * @file
 * @brief   Variable configuration header depending on the parameters when the ./configure script is started
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_AUTOCONFIG_H
#define VTAPI_AUTOCONFIG_H 1
])
AH_BOTTOM([#endif /* VTAPI_AUTOCONFIG_H */])


###############################################################################
#####   Register configure arguments and variables                        #####
###############################################################################

### Configure variables
AC_ARG_VAR(GEOS_CONFIG, [GEOS geos_config file which will be used to build with.])
AC_ARG_VAR(LIBPQTYPES_PREFIX, [libpqtypes prefix which will be used to build with.])
AC_ARG_VAR(PG_CONFIG, [PostgreSQL pg_config file which will be used to build with.])
AC_ARG_VAR(SQLITE_PC, [SQLite sqlite3.pc file which will be used to build with.])

### Configure with arguments
AC_ARG_WITH([geosconfig], 
            [AS_HELP_STRING([--with-geosconfig=FILE], [specify an alternative geos_config file])],
            [GEOS_CONFIG="${withval}"], 
            [])
AC_ARG_WITH([libpqtypes-prefix], 
            [AS_HELP_STRING([--with-libpqtypes-prefix=PREFIX], [specify an alternative prefix for libpqtypes files])],
            [LIBPQTYPES_PREFIX="${withval}"], 
            [])
AC_ARG_WITH([pgconfig], 
            [AS_HELP_STRING([--with-pgconfig=FILE], [specify an alternative pg_config file])],
            [PG_CONFIG="${withval}"], 
            [])
AC_ARG_WITH([sqlitepc],
            [AS_HELP_STRING([--with-sqlitepc=FILE], [specify an alternative sqlite3.pc file])],
            [SQLITE_PC="${withval}"], 
            [])

### Configure without arguments
AC_ARG_WITH([opencv], AS_HELP_STRING([--without-opencv], [compile without OpenCV support]))
AC_ARG_WITH([pgsql], AS_HELP_STRING([--without-pgsql], [compile without PostgreSQL support (i.e. force SQLite)]))
AC_ARG_WITH([postgis], AS_HELP_STRING([--without-postgis], [compile without PostGIS support]))
AC_ARG_WITH([postgresql], AS_HELP_STRING([--without-postgresql], [alias for --without-pgsql]))
AC_ARG_WITH([sqlite], AS_HELP_STRING([--without-sqlite], [compile without SQLite support (i.e. force PostgreSQL)]))

### Configure enable arguments
AC_ARG_ENABLE([doc], AS_HELP_STRING([--enable-doc], [Build the documentation]))



###############################################################################
#####   Core tools presence check                                         #####
###############################################################################

### ltdl => yum install libtool-ltdl-devel (urpmi libtool-base)
AC_CHECK_HEADER(ltdl.h, [], [AC_MSG_ERROR([the ltdl.h header is missing. Please install package libtool-ltdl-devel (libtool-base, ..).])])
LTDL_CFLAGS=""
LTDL_LIBS="-lltdl"
AC_SUBST([LTDL_CFLAGS])
AC_SUBST([LTDL_LIBS])

### pkg-config
AC_PATH_PROG([PKG_CONFIG], [pkg-config], [AC_MSG_ERROR([could not find pkg-config. You may need to install pkg-config.])])



###############################################################################
#####   Core tools - some useful functions                                #####
###############################################################################

### Strip absolute path of *.so libs (/path/to/lib/libXYZ.so) to format "-L/path/to/lib -lXYZ" for LD
function stripLibsAbsolutePath () {
  local TMP_LIBS=`echo $1 | sed -r 's@([[^ ]]*/)?lib([[-_.A-Za-z0-9]]+)\.so(\s|$)@ \-l\2 @g'`
  local TMP_LDIR=`echo $1 | grep -o -E '[[^ ]]+/lib[[-_.A-Za-z0-9]]+\.so' | head -n 1`
  if test "x${TMP_LDIR}" != "x"
  then
    TMP_LDIR=`echo ${TMP_LDIR} | sed -r 's@([[^ ]]+)/lib[[-_.A-Za-z0-9]]+\.so@\-L\1@'`
    TMP_LIBS="${TMP_LDIR} ${TMP_LIBS}"
  fi
  echo "${TMP_LIBS}"
}



###############################################################################
#####   OpenCV presence check                                             #####
###############################################################################
opencv_want="yes"

# PKG_CHECK_MODULES([opencv], [opencv >= 2.4.1], [], [opencv_present="no"])

AS_IF([test "x${with_opencv}" = "xno"], [opencv_want="no"])

if test "x${opencv_want}" = "xyes"
then
  opencv_present="yes"
  OPENCV_VERSION=`"${PKG_CONFIG}" opencv --modversion` 
  if test -z "${OPENCV_VERSION}"
  then
    opencv_present="no"
  else 
    OPENCV_CFLAGS=`"${PKG_CONFIG}" opencv --cflags`
    OPENCV_LIBS=`stripLibsAbsolutePath "$("${PKG_CONFIG}" opencv --libs)"`
    AC_SUBST([OPENCV_CFLAGS])
    AC_SUBST([OPENCV_LIBS])
  fi

  if test "x${opencv_present}" = "xno"
  then
    AC_MSG_ERROR([could not find OpenCV. You may need to install OpenCV or re-run configure with a --without-opencv argument.])
  fi

  AC_DEFINE([HAVE_OPENCV], [1], [opencv is present])
  OPENCV_SUMMARY="${opencv_present}   (version: ${OPENCV_VERSION})"
  
else
  AC_MSG_NOTICE([OpenCV support disabled (by user through --without-opencv argument).])
  OPENCV_SUMMARY="disabled" # by user (through --without-opencv)"
fi



###############################################################################
#####   SQLite presence check                                             #####
###############################################################################
sqlite_want="yes"

#PKG_CHECK_MODULES([sqlite3], [sqlite3 >= 3.7.0], [],[sqlite_present="no"])

AS_IF([test "x${with_sqlite}" = "xno"], [sqlite_want="no"])

if test "x${sqlite_want}" = "xyes"
then
  sqlite_present="yes"
  
  if test "x${SQLITE_PC}" = "x"
  then
    # try to search in current path
    package=sqlite3
    SQLITE_VERSION=`"${PKG_CONFIG}" "${package}" --modversion`
    if test "x${SQLITE_VERSION}" = "x"
    then
      sqlite_present="no"
    fi
  else
    # we have specific sqlite3.pc path - so we must check it
    if test -f "${SQLITE_PC}"
    then
      AC_MSG_RESULT([Using user-specified sqlite3.pc file: ${SQLITE_PC}])    
    else
      AC_MSG_ERROR([the user-specified sqlite3.pc file (${SQLITE_PC}) does not exist - please, check the specified path or do not set --with-sqlitepc argument.])
      sqlite_present="no"
    fi
    package=${SQLITE_PC}
  fi
  
  if test "x${sqlite_present}" = "xyes"
  then
    SQLITE_VERSION=`"${PKG_CONFIG}" "${package}" --modversion`
    SQLITE_CFLAGS=`"${PKG_CONFIG}" "${package}" --cflags`
    SQLITE_LIBS=`"${PKG_CONFIG}" "${package}" --libs`
    SQLITE_LIBDIR=`"${PKG_CONFIG}" "${package}" --variable=libdir`
    
    TMP_CPPFLAGS="${CPPFLAGS}"
    CPPFLAGS="${SQLITE_CFLAGS} ${CPPFLAGS}"
    TMP_LIBS="${LIBS}"
    LIBS="${SQLITE_LIBS} ${LIBS}"

    AC_CHECK_HEADER(sqlite3.h, [], [sqlite_present="no"])
    AC_CHECK_LIB([sqlite3], [sqlite3_open], [], [sqlite_present="no"])
    
    CPPFLAGS="${TMP_CPPFLAGS}"
    LIBS="${TMP_LIBS}"
  fi

  SQLITE_SUMMARY="${sqlite_present}"
  if test "x${sqlite_present}" = "xyes" 
  then
    AC_DEFINE([HAVE_SQLITE], [1], [SQLite is present])
    AC_DEFINE_UNQUOTED([SQLITE_LIB_PATH], ["${SQLITE_LIBDIR}"], [SQLite library path])
    AC_SUBST([SQLITE_CFLAGS])
    AC_SUBST([SQLITE_LIBS])
    SQLITE_SUMMARY="${SQLITE_SUMMARY}   (version: ${SQLITE_VERSION})"
  fi
  
else
  AC_MSG_NOTICE([SQLite support disabled (by user through --without-sqlite argument).])
  SQLITE_SUMMARY="disabled" # by user (through --without-sqlite)"
fi


# FIXME 
# dalsi veci pro postgis
# pgDistance, cube, pgCubeBox3d



###############################################################################
#####   PostgreSQL and its components presence check                      #####
###############################################################################
pgsql_want="yes"

AS_IF([test "x${with_pgsql}" = "xno"], [pgsql_want="no"])
AS_IF([test "x${with_postgresql}" = "xno"], [pgsql_want="no"])

if test "x${pgsql_want}" = "xyes"
then
  pgsql_present="yes"
  if test "x${PG_CONFIG}" = "x"
  then
    # try to search in current path
    AC_PATH_PROG([PG_CONFIG], [pg_config])
    if test "x${PG_CONFIG}" = "x"
    then
      pgsql_present="no"
    fi
  else
    # we have specific pg_config path - so we must check it
    if test -f "${PG_CONFIG}"
    then
      AC_MSG_RESULT([Using user-specified pg_config file: ${PG_CONFIG}])    
    else
      AC_MSG_ERROR([the user-specified pg_config file (${PG_CONFIG}) does not exist - please, check the specified path or do not set --with-pgconfig argument.])
      pgsql_present="no"
    fi
  fi

  if test "x${pgsql_present}" = "xyes"
  then
    PGSQL_VERSION=`${PG_CONFIG} --version`
    PGSQL_LIBDIR=`${PG_CONFIG} --libdir`
    PQ_CFLAGS="-I$("${PG_CONFIG}" --includedir)"
    PQ_LIBS="-L${PGSQL_LIBDIR} -lpq -lpqtypes"
  
    TMP_CPPFLAGS="${CPPFLAGS}"
    CPPFLAGS="${PQ_CFLAGS} ${CPPFLAGS}"
    TMP_LIBS="${LIBS}"
    LIBS="${PQ_LIBS} ${LIBS}"
    AC_CHECK_HEADER(libpq-fe.h, [], [pgsql_present="no"]) 
    AC_CHECK_LIB([pq], [PQserverVersion], [], [pgsql_present="no"]) # ??? is this OK on Windows???
    if test "x${LIBPQTYPES_PREFIX}" != "x"
    then
      LIBPQTYPES_PREFIX=${LIBPQTYPES_PREFIX%/}
      AC_MSG_RESULT([Using user-specified libpqtypes prefix: ${LIBPQTYPES_PREFIX}])  
      LIBPQTYPES_CPPFLAGS="-I${LIBPQTYPES_PREFIX}/include"
      LIBPQTYPES_LIBDIR="${LIBPQTYPES_PREFIX}/lib"
      LIBPQTYPES_LIBS="-L${LIBPQTYPES_LIBDIR}"
      CPPFLAGS="${LIBPQTYPES_CPPFLAGS} ${CPPFLAGS}"
      LIBS="${LIBPQTYPES_LIBS} ${LIBS}"
    fi
    AC_CHECK_HEADER(libpqtypes.h, [], [pgsql_present="no"])
    AC_CHECK_LIB([pqtypes], [PQinitTypes], [], [pgsql_present="no"])
    CPPFLAGS="${TMP_CPPFLAGS}"
    LIBS="${TMP_LIBS}"
  fi

  PGSQL_SUMMARY="${pgsql_present}"
  
  if test "x${pgsql_present}" = "xno"
  then
    AC_MSG_WARN([Postgresql is not available.])
  else 
    AC_DEFINE([HAVE_POSTGRESQL], [1], [PostgreSQL is present])
    AC_DEFINE_UNQUOTED([PG_LIB_PATH], ["${PGSQL_LIBDIR}"], [PostgreSQL library path])
    AC_DEFINE_UNQUOTED([PQTYPES_LIB_PATH], ["${LIBPQTYPES_LIBDIR}"], [libpqtypes library path])
    AC_SUBST([PQ_CFLAGS])
    AC_SUBST([PQ_LIBS])
    AC_SUBST([LIBPQTYPES_CPPFLAGS])
    AC_SUBST([LIBPQTYPES_LIBS])
    PGSQL_SUMMARY="${PGSQL_SUMMARY}   (version: ${PGSQL_VERSION})"
  fi
  
else
  AC_MSG_NOTICE([PostgreSQL support disabled (by user through --without-pgsql argument).])
  PGSQL_SUMMARY="disabled" # by user (through --without-pgsql)"
fi


###############################################################################
#####   Database backend presence check                                   #####
#####     -> PostgreSQL or SQLite support is necessary                    #####
###############################################################################
if test "x${pgsql_present}" != "xyes" -a "x${sqlite_present}" != "xyes"
then
    AC_MSG_ERROR([could not find some supported database backend - VTApi needs PostgreSQL or SQLite.])
fi



###############################################################################
#####   PostGIS and its components presence check                         #####
###############################################################################
postgis_want="yes"
postgis_want="disabled" # FIXME: small hack due to postgis problems - for development, comment this line

AS_IF([test "x${with_postgis}" = "xno"], [postgis_want="no"])

if test "x${pgsql_want}" = "xyes" -a "x${postgis_want}" = "xyes"
then
  postgis_present="yes"

  ### PostGIS - liblwgeom => -llwgeom
  lwgeom_present="yes"
#  AC_CHECK_HEADER(liblwgeom.h, [], [lwgeom_present="no"]) TODO: use postgres/liblwgeom/liblwgeom.h from vtapi or liblwgeom.h from original package??
  AC_CHECK_LIB([lwgeom], [lwgeom_to_ewkt], [], [lwgeom_present="no"])


  ### PostGIS - GEOS => -lgeos_c
  geos_present="yes"
  
  if test "x${GEOS_CONFIG}" = "x"
  then
    # try to search in current path
    AC_PATH_PROG([GEOS_CONFIG], [geos-config])
    if test "x${GEOS_CONFIG}" = "x"
    then
      geos_present="no"
      AC_MSG_WARN([could not find geos_config. You may need to install GEOS or re-run configure with a --with-geosconfig argument.])
    fi
  else
    # we have specific geos_config path - so we must check it
    if test -f "${GEOS_CONFIG}"
    then
      AC_MSG_RESULT([Using user-specified geos_config file: ${GEOS_CONFIG}])    
    else
      AC_MSG_ERROR([the user-specified geos_config file (${GEOS_CONFIG}) does not exist - please, check the specified path or do not set --with-geosconfig argument.])
      geos_present="no"
    fi
  fi
  
  GEOS_SUMMARY="${geos_present}"

  if test "x${geos_present}" = "xyes"
  then
    GEOS_VERSION=`${GEOS_CONFIG} --version`
    GEOS_MAJOR=`${GEOS_CONFIG} --version | cut -d. -f1`
    GEOS_MINOR=`${GEOS_CONFIG} --version | cut -d. -f2`
    GEOS_VERNUM=`expr ${GEOS_MAJOR} "*" 10`
    GEOS_VERNUM=`expr ${GEOS_VERNUM} "+" ${GEOS_MINOR}`

    if test "${GEOS_VERNUM}" -lt 32 # GEOS older than 3.2.0 is probably problem..
    then
      AC_MSG_ERROR([GEOS should be at least 3.2.0 or newer (${POSTGIS_VERSION} was given).])
    fi

    AC_MSG_CHECKING([for GEOS include directory])
    GEOS_CFLAGS=`"${GEOS_CONFIG}" --cflags`
    AC_MSG_RESULT([${GEOS_CFLAGS}])
    AC_SUBST([GEOS_CFLAGS])

    AC_MSG_CHECKING([for GEOS libs])
    GEOS_LIBS=`"${GEOS_CONFIG}" --libs`
    AC_MSG_RESULT([${GEOS_LIBS}])
    AC_SUBST([GEOS_LIBS])

    AC_DEFINE([HAVE_GEOS], [1], [geos is present])
    GEOS_SUMMARY="${GEOS_SUMMARY}   (version: ${GEOS_VERSION})"
  fi

  ### PostGIS
  if test "x${geos_present}" != "xyes" -o "x${lwgeom_present}" != "xyes"
  then
    postgis_present="no"
  fi

  POSTGIS_SUMMARY="${postgis_present}"

  if test "x${postgis_present}" = "xyes"
  then
    AC_DEFINE([HAVE_POSTGIS], [1], [geos, lwgeom, and postgres are available])
    if test "x${pgsql_present}" != "xyes"
    then
      POSTGIS_SUMMARY="${POSTGIS_SUMMARY} (but PostgreSQL missing)"
    fi
  else
    if test "x${pgsql_present}" = "xyes"
    then
      AC_MSG_ERROR([could not find all dependencies for PostGIS. You may need to install all dependencies or re-run configure with --without-postgis argument.])
    else
      AC_MSG_WARN([could not find all dependencies for PostGIS. You may need to install all dependencies.])
    fi
  fi

elif test "x${pgsql_want}" = "xno"
then
  AC_MSG_NOTICE([PostGIS support disabled due to disabled PostgreSQL (by user through --without-pgsql argument).])
  POSTGIS_SUMMARY="disabled due to disabled PostgreSQL" # by user (through --without-postgis)"
elif test "x${postgis_want}" = "xdisabled"
then
  AC_MSG_NOTICE([PostGIS support hard-disabled (There's some issues on postgis support).])
  POSTGIS_SUMMARY="hard-disabled (due to some issues)"
else
  AC_MSG_NOTICE([PostGIS support disabled (by user through --without-postgis argument).])
  POSTGIS_SUMMARY="disabled" # by user (through --without-postgis)"
fi




###############################################################################
#####   Documentation of VTApi                                            #####
###############################################################################
DOC_SUMMARY="no"
AS_IF([test "x${enable_doc}" = "xyes"], [
  AC_CHECK_PROG(HAVE_DOXYGEN, doxygen, true)
  AC_SUBST([HAVE_DOXYGEN])
  if test "x${HAVE_DOXYGEN}" = "x"
  then
    AC_MSG_WARN([could not find Doxygen. You may need to install doxygen to build the documentation.])
    DOC_SUMMARY="${DOC_SUMMARY} (Doxygen was not found)"
  else
    AC_CONFIG_FILES([doc/Makefile doc/Doxyfile doc/MAINPAGE.txt])
    DOC_SUMMARY="yes"
  fi
])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$HAVE_DOXYGEN"]) # prenese do Makefile.am podminku - HAVE_DOXYGEN se testuje jen pokud je povoleny



###############################################################################
#####   Additional checks                                                 #####
###############################################################################

### readproc => yum install procps-ng-devel (urpmi procps-devel)
readproc_present="yes"
AC_CHECK_HEADER(proc/readproc.h, [], [readproc_present="no"])
AC_CHECK_LIB([procps], [readproctab], [READPROC_LIBS="-lprocps"])
AC_CHECK_LIB([proc], [readproctab], [READPROC_LIBS="-lproc"])

if test -z "${READPROC_LIBS}"
then
  readproc_present="no"
fi

if test "x${readproc_present}" = "xyes"
then
  AC_DEFINE([HAVE_READPROC], [1], [readproc is present])
  
  READPROC_CFLAGS=""
  
  AC_SUBST([READPROC_CFLAGS])
  AC_SUBST([READPROC_LIBS])
fi


### Checks for typedefs, structures, and compiler characteristics.
# AC_CHECK_HEADER_STDBOOL
# AC_TYPE_SIZE_T

### Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset select strchr strcspn strspn strtol])



###############################################################################
#####   Platform-dependent commands                                       #####
###############################################################################
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



###############################################################################
#####   PREPARE OUTPUT                                                    #####
###############################################################################
AC_CONFIG_FILES([src/Makefile include/Makefile Makefile])
#AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile])
AC_OUTPUT



###############################################################################
#####   VTAPI CONFIGURE SUMMARY                                           #####
###############################################################################

AC_MSG_RESULT()
AC_MSG_RESULT()
AC_MSG_RESULT([ ---------- VTApi ${VTAPI_API_VERSION} configure summary ----------])
AC_MSG_RESULT([  OpenCV:         ${OPENCV_SUMMARY}])
AC_MSG_RESULT([  SQLite:         ${SQLITE_SUMMARY}])
if test "x${sqlite_present}" = "xyes" -a "x${SQLITE_PC}" != "x"
then
  AC_MSG_RESULT([                    -> sqlite3.pc custom path: ${SQLITE_PC}])
fi

AC_MSG_RESULT([  PostgreSQL:     ${PGSQL_SUMMARY]})
if test "x${pgsql_present}" = "xyes"
then
  AC_MSG_RESULT([                    -> pg_config path: ${PG_CONFIG}])
  if test "x${LIBPQTYPES_PREFIX}" != "x"
  then
    AC_MSG_RESULT([                    -> libpqtypes prefix: ${LIBPQTYPES_PREFIX}])
  fi
fi

AC_MSG_RESULT([  PostGIS:        ${POSTGIS_SUMMARY]})

if test "x${postgis_present}" != "x"
then
  AC_MSG_RESULT([   - liblwgeom:   ${lwgeom_present}])
  AC_MSG_RESULT([   - geos:        ${GEOS_SUMMARY}])
fi

if test "x${geos_present}" = "xyes"
then
  AC_MSG_RESULT([                    -> geos_config path: ${GEOS_CONFIG}])
fi

AC_MSG_RESULT([  proc/procps:    ${readproc_present}])
AC_MSG_RESULT([  documentation:  ${DOC_SUMMARY}])
AC_MSG_RESULT()
AC_MSG_RESULT()
