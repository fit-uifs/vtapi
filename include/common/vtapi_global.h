/**
 * @file
 * @brief   Definition of %VTApi core return codes, errors and destructors
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

#include "vtapi_config.h"


// DESTRUCTORS (..be nice while destructing)
/**
 * @brief VTApi object dealocator
 */
#define vt_destruct(v)         if (v) { delete(v); (v) = NULL; }
/**
 * @brief VTApi array of object dealocator
 */
#define vt_destructall(v)      if (v) { delete[](v); (v) = NULL; }
