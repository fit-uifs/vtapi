/**
 * @file
 * @brief   Definition of %VTApi core return codes, errors and destructors
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_GLOBAL_H
#define	VTAPI_GLOBAL_H

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


// RETURN CODES
/**
 * @brief VTApi success return code
 */
#define VT_OK               true
/**
 * @brief VTApi failure return code
 */
#define VT_FAIL             false


// ERRORS
/**
 * @brief VTApi general failure error code
 */
#define ER_FAIL             -1
/**
 * @brief VTApi wrong query error code
 */
#define ER_WRONG_QUERY      -2


// WARNINGS



#endif	/* VTAPI_GLOBALS_H */

