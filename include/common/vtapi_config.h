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

#ifndef VTAPI_CONFIG_H
#define VTAPI_CONFIG_H

// by configure script
#include "vtapi_autoconfig.h"

// standard library
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>

// boost
#include <boost/interprocess/ipc/message_queue.hpp>

// c++11
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>

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

#if HAVE_OPENCV
// OpenCV header files
 #include <opencv2/opencv.hpp>
#endif 

#if HAVE_POSTGIS

#if HAVE_GEOS
// GEOS 3.3.3 (http://trac.osgeo.org/geos/) - C wrapper
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


#endif	/* VTAPI_CONFIG_H */

