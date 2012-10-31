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


// comment this, if there is no PostGIS extension in the database
#define POSTGIS

// comment this under compilers with no copyfmt/rdbuf capabilities (GCC4.6 @ merlin)
#define COPYRDBUF

// libpqtypes
#include <libpqtypes.h> // tohle se pak poresi configure

// libproc library
// for this you need the libproc-dev (sometimes procps-devel) package
#if !(defined(WIN32) || defined(WIN64))
#include <proc/readproc.h>
#endif

// OpenCV
// #include <opencv2/core/core.hpp>
// #include <opencv2/core/core_c.h>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/highgui/highgui.hpp>

// GEOS 3.3.3 (http://trac.osgeo.org/geos/) - C wrapper
#include <geos_c.h>

// postGIS ... nove je verze 2.0.0
#include "../postgres/liblwgeom/liblwgeom.h"
#include "../postgres/cube/cubedata.h"
// TODO: rozhodnout se, co s timhle kodem udelat ... ?

#endif	/* VTAPI_CONFIG_H */

