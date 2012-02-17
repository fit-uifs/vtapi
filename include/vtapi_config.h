/**
 * @file
 * @authors
 * VTApi Team, FIT BUT, CZ
 * Petr Chmelar, chmelarp@fit.vutbr.cz
 * Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * Tomas Volf, ivolf@fit.vutbr.cz
 *
 *
 * @section LECENSE License
 *
 * There will be license information for VTApi.
 * &copy; FIT BUT, CZ, 2011
 *
 *
 * @section DESCRIPTION Description
 *
 * Optional VTApi configuration for includes. Comment #includes to make VTApi
 * more lightweight.
 *
 */

#ifndef VTAPI_CONFIG_H
#define VTAPI_CONFIG_H


// libpqtypes
#include "postgresql/libpqtypes.h"
#include "vtapi_libpq.h"

// libproc library
// for this you need the libproc-dev (sometimes procps-devel) package
#include <proc/readproc.h>

// comment this under compilers with no copyfmt/rdbuf capabilities (GCC4.6 @ merlin)
#define COPYRDBUF

// comment this, if there is no PostGIS extension in the database
#define POSTGIS

// openCV 
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// postGIS
//#include "postgis/liblwgeom.h"

// GEOS 3.2.3 (http://trac.osgeo.org/geos/)
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
//#include <geos/geom/Point.h>

#endif	/* VTAPI_CONFIG_H */

