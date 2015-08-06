/**
 * @file
 * @brief   Definition of %VTApi core return codes, errors and destructors
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "vtapi_config.h"
#include "vtapi_defs.h"


// for global logging
#include <common/vtapi_logger.h>


#define PG_FORMAT 1   // postgres data transfer format: 0=text, 1=binary

// sqlite database files
#define SL_DB_PREFIX "vtapi_"
#define SL_DB_SUFFIX ".db"
#define SL_DB_PUBLIC "public"



// DESTRUCTORS (..be nice while destructing)
/**
 * @brief VTApi object dealocator
 */
#define vt_destruct(v)         if (v) { delete(v); (v) = NULL; }
/**
 * @brief VTApi array of object dealocator
 */
#define vt_destructall(v)      if (v) { delete[](v); (v) = NULL; }
