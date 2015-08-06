/**
 * @file
 * @brief   %VTApi configuration header for includes based on vtapi_autoconfig.h header.
 *
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

// platform independent library loading
#include <ltdl.h>

// libpqtypes + pq
#if VTAPI_HAVE_POSTGRESQL
    #include <libpqtypes.h>
#endif

// sqlite
#if VTAPI_HAVE_SQLITE
    #include <sqlite3.h>
#endif

// OpenCV header files
#if VTAPI_HAVE_OPENCV
 #include <opencv2/opencv.hpp>
#endif 

#if VTAPI_HAVE_POSTGIS

// GEOS 3.3.3 (http://trac.osgeo.org/geos/) - C wrapper
#if VTAPI_HAVE_GEOS
#ifdef	__cplusplus
extern "C" {
#endif
    #include <geos_c.h>
#ifdef	__cplusplus
}
#endif
#endif // VTAPI_HAVE_GEOS

// postGIS 2.0
#ifdef	__cplusplus
extern "C" {
#endif
    #include "../postgres/liblwgeom/liblwgeom.h"
    #include "../postgres/cube/cubedata.h"
#ifdef	__cplusplus
}
#endif // __cplusplus

#endif // VTAPI_HAVE_POSTGIS
