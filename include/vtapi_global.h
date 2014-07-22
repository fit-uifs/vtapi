/* 
 * File:   vtapi_global.h
 * Author: vojca
 *
 * Created on May 7, 2013, 3:35 PM
 */

#ifndef VTAPI_GLOBAL_H
#define	VTAPI_GLOBAL_H


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
#include <typeinfo>
#include <time.h>

// use stuff from standard library
using std::string;
using std::vector;
using std::map;
using std::set;
using std::pair;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;

// be nice while destructing
#define destruct(v)         if (v) { delete(v); (v) = NULL; }
#define destructall(v)      if (v) { delete[](v); (v) = NULL; }

// generic stuff used everywhere
#include "common/vtapi_config.h"
#include "common/vtapi_settings.h"
#include "common/vtapi_errcodes.h"
#include "common/vtapi_serialize.h"
#include "common/vtapi_tkeyvalue.h"
#include "common/vtapi_logger.h"

namespace vtapi {

// dynamically loaded functions
typedef map<string,void *> fmap_t;

// backend type
typedef enum {
    UNKNOWN = 0,
    SQLITE,
    POSTGRES
} backend_t;





} //namespace vtapi

#endif	/* VTAPI_GLOBALS_H */

