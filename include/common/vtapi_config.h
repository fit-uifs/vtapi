/**
 * @file
 * @brief   %VTApi configuration header for includes based on vtapi_autoconfig.h header.
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

// by configure script
#include "vtapi_autoconfig.h"


/**
 * @brief TODO
 * 
 * @todo @b doc: put together a few letters..
 * 
 * @note comment this under compilers with no copyfmt/rdbuf capabilities (GCC4.6 @ merlin)
 */
#define COPYRDBUF

// platform independent library loading
#include <ltdl.h>

// libpqtypes + pq
#if HAVE_POSTGRESQL
    #include <libpqtypes.h>

    #define PG_FORMAT 1   // postgres data transfer format: 0=text, 1=binary
#endif

// sqlite
#if HAVE_SQLITE
    #include <sqlite3.h>

    // sqlite database files
    #define SL_DB_PREFIX "vtapi_"
    #define SL_DB_SUFFIX ".db"
    #define SL_DB_PUBLIC "public"
#endif


// libproc library
// for this you need the libprocps-dev (sometimes libproc-dev, procps-devel) package
#if HAVE_READPROC
  #include <proc/readproc.h>
#endif

// OpenCV header files
#if HAVE_OPENCV
 #include <opencv2/opencv.hpp>
#endif 

#if HAVE_POSTGIS

// GEOS 3.3.3 (http://trac.osgeo.org/geos/) - C wrapper
#if HAVE_GEOS
#ifdef	__cplusplus
extern "C" {
#endif
    #include <geos_c.h>
#ifdef	__cplusplus
}
#endif
#endif // HAVE_GEOS

// postGIS 2.0
#ifdef	__cplusplus
extern "C" {
#endif
    #include "../postgres/liblwgeom/liblwgeom.h"
    #include "../postgres/cube/cubedata.h"
#ifdef	__cplusplus
}
#endif // __cplusplus

#endif // HAVE_POSTGIS
