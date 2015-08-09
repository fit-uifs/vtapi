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



// global logging
#include "logger.h"


// delete macro helper
#define vt_destruct(v)         if (v) { delete(v); (v) = NULL; }
// delete[] macro helper
#define vt_destructall(v)      if (v) { delete[](v); (v) = NULL; }
