/* 
 * File:   vtapi_global.h
 * Author: vojca
 *
 * Created on May 7, 2013, 3:35 PM
 */

#ifndef VTAPI_GLOBAL_H
#define	VTAPI_GLOBAL_H

#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>

#include "vtapi_config.h"
#include "vtapi_errcodes.h"


// be nice while destructing
#define destruct(v)         if (v) { delete(v); (v) = NULL; }
#define destructall(v)      if (v) { delete[](v); (v) = NULL; }

// sqlite database files
#define SL_DB_PREFIX        "vtapi_"
#define SL_DB_SUFFIX        ".db"
#define SL_DB_PUBLIC        "public"

// postgres data transfer format: 0=text, 1=binary
#define PG_FORMAT           1

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


namespace vtapi {


/**
 * A generic function to convert any numeric type to string
 * (any numeric type, e.g. int, float, double, etc.)
 * @param t
 * @return string
 */
template <class T>
inline string toString(const T& value) {
    std::ostringstream ostr;
    ostr << value;
    return ostr.str();
};

template <>
inline string toString <time_t>(const time_t& value) {
    char buff[20];
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", gmtime(&value));
    return string(buff);
};

inline time_t toTimestamp(const string& value) {
    struct tm ts = {0};
    sscanf(value.c_str(), "%d-%d-%d %d:%d:%d", &ts.tm_year, &ts.tm_mon, &ts.tm_mday, &ts.tm_hour, &ts.tm_min, &ts.tm_sec);
    ts.tm_year -= 1900;
    return mktime(&ts);
};

template<class T>
inline T* deserializeA(char *buffer, int& size) {
    T *ret  = NULL;
    string valStr   = string(buffer);     
    
    if ((valStr.find('[', 0) == 0) && (valStr.find(']', 0) == valStr.length()-1)) {
        size_t leftPos  = 1;
        size_t nextPos  = 1;
        size            = 1;
        leftPos         = valStr.find(',', 1);
        while ( leftPos != string::npos) {
            size++;
            leftPos     = valStr.find(',', leftPos+1);
        }
        
        ret         = new T[size];
        leftPos     = 1;
        for (int i = 0; i < size; i++) {
            nextPos = valStr.find_first_of(",]", leftPos+1);
            stringstream(valStr.substr(leftPos, nextPos-leftPos)) >> ret[i];
            leftPos     = nextPos + 1;
        }
        
    }
    else {
        ret     = new T[1];
        size    = 1;
        stringstream(valStr) >> ret[0];
    }

    return ret;
}

template<class T>
inline vector<T>* deserializeV(char *buffer) {
    vector<T> *ret  = NULL;
    string valStr   = string(buffer);

    if ((valStr.find('[', 0) == 0) && (valStr.find(']', 0) == valStr.length()-1)) {
        size_t leftPos  = 1;
        size_t nextPos  = 1;
        int size        = 1;
        leftPos         = valStr.find(',', 1);
        while ( leftPos != string::npos) {
            size++;
            leftPos     = valStr.find(',', leftPos+1);
        }

        ret         = new vector<T>;
        leftPos     = 1;
        for (int i = 0; i < size; i++) {
            T val;
            nextPos = valStr.find_first_of(",]", leftPos+1);
            stringstream(valStr.substr(leftPos, nextPos-leftPos)) >> val;
            ret->push_back(val);
            leftPos     = nextPos + 1;
        }

    }
    else {
        T val;
        ret     = new vector<T>;        
        stringstream(valStr) >> val;
        ret->push_back(val);
    }

    return ret;
}



} //namespace vtapi

#endif	/* VTAPI_GLOBALS_H */

