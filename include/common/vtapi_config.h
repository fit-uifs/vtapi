/**
 * @file
 * @brief Optional %VTApi configuration for includes. Comment \#includes to make
 * %VTApi more lightweight..
 *
 * @copyright Brno University of Technology &copy; 2011 &ndash; 2012
 *
 * VTApi is distributed under BUT OPEN SOURCE LICENCE (Version 1).
 * This licence agreement provides in essentials the same extent of rights as the terms of GNU GPL version 2 and Software fulfils the requirements of the Open Source software.
 *
 * @authors
 * Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 */

#ifndef VTAPI_CONFIG_H
#define VTAPI_CONFIG_H

// by configure script
#include "vtapi_autoconfig.h"

// standard library
#include <iomanip>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <typeinfo>

// comment this under compilers with no copyfmt/rdbuf capabilities (GCC4.6 @ merlin)
#define COPYRDBUF

// platform independent library loading
#include <ltdl.h>

// libpqtypes + pq
#if HAVE_POSTGRESQL
  #include <libpqtypes.h> 
#endif

// sqlite
#if HAVE_SQLITE
  #include <sqlite3.h>
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

