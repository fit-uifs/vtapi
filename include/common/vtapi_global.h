/* 
 * File:   vtapi_global.h
 * Author: vojca
 *
 * Created on May 7, 2013, 3:35 PM
 */

#ifndef VTAPI_GLOBAL_H
#define	VTAPI_GLOBAL_H

#include "vtapi_config.h"

// be nice while destructing
#define vt_destruct(v)         if (v) { delete(v); (v) = NULL; }
#define vt_destructall(v)      if (v) { delete[](v); (v) = NULL; }

// RETURN CODES

#define VT_OK               true
#define VT_FAIL             false

// ERRORS

#define ER_FAIL             -1
#define ER_WRONG_QUERY      -2


// WARNINGS



#endif	/* VTAPI_GLOBALS_H */

