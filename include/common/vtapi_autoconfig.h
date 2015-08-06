/* include/common/vtapi_autoconfig.h.  Generated from vtapi_autoconfig.h.in by configure.  */
/* include/common/vtapi_autoconfig.h.in.  Generated from configure.ac by autoheader.  */

/**
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
#define VTAPI_AUTOCONFIG_H


/* define if the Boost library is available */
#define HAVE_BOOST /**/

/* define if the Boost::Filesystem library is available */
#define HAVE_BOOST_FILESYSTEM /**/

/* define if the Boost::System library is available */
#define HAVE_BOOST_SYSTEM /**/

/* define if the compiler supports basic C++11 syntax */
/* #undef HAVE_CXX11 */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* geos is present */
/* #undef VTAPI_HAVE_GEOS */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `lwgeom' library (-llwgeom). */
/* #undef HAVE_LIBLWGEOM */

/* Define to 1 if you have the `pq' library (-lpq). */
#define HAVE_LIBPQ 1

/* Define to 1 if you have the `pqtypes' library (-lpqtypes). */
#define HAVE_LIBPQTYPES 1

/* Define to 1 if you have the `sqlite3' library (-lsqlite3). */
#define HAVE_LIBSQLITE3 1

/* Define to 1 if you have the `xml++-2.6' library (-lxml++-2.6). */
#define HAVE_LIBXML___2_6 1

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* opencv is present */
#define VTAPI_HAVE_OPENCV 1

/* geos, lwgeom, and postgres are available */
/* #undef VTAPI_HAVE_POSTGIS */

/* PostgreSQL is present */
#define VTAPI_HAVE_POSTGRESQL 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* SQLite is present */
#define VTAPI_HAVE_SQLITE 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strcspn' function. */
#define HAVE_STRCSPN 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strspn' function. */
#define HAVE_STRSPN 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* libXML++ is present */
#define VTAPI_HAVE_XMLXX 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "vtapi"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "ivolf@fit.vutbr.cz"

/* Define to the full name of this package. */
#define PACKAGE_NAME "VTApi"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "VTApi 2.5"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "vtapi"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://vidte.fit.vutbr.cz"

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.5"

/* PostgreSQL library path */
#define VTAPI_PG_LIB_PATH "/usr/lib/x86_64-linux-gnu"

/* libpqtypes library path */
#define VTAPI_PQTYPES_LIB_PATH "/usr/lib/x86_64-linux-gnu"

/* SQLite library path */
#define VTAPI_SQLITE_LIB_PATH "/usr/lib/x86_64-linux-gnu"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.5"

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

#endif /* VTAPI_AUTOCONFIG_H */
