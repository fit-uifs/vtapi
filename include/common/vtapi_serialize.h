/**
 * @file
 * @brief   Definition of serialization and deserialization functions
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_SERIALIZE_H
#define	VTAPI_SERIALIZE_H

#include "../data/vtapi_intervalevent.h"

namespace vtapi {

/**
 * A generic function to convert any numeric type to string
 * (any numeric type, e.g. int, float, double, etc.)
 * @param value   numeric value
 * @return string containing the numeric value
 */
template <class T>
inline std::string toString(const T& value) {
    std::ostringstream ostr;
    ostr << value;
    return ostr.str();
};

/**
 * Converts timestamp to string (yyyy-mm-dd hh:mm:ss)
 * @param value   time value
 * @return time string
 */
template <>
inline std::string toString <time_t>(const time_t& value) {
    char buff[20];
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", gmtime(&value));
    return std::string(buff);
};

/**
 * Converts time string (yyyy-mm-dd hh:mm:ss) to timestamp
 * @param value   time string
 * @return timestamp
 */
inline time_t toTimestamp(const std::string& value) {
    struct tm ts = {0};
    sscanf(value.c_str(), "%d-%d-%d %d:%d:%d", &ts.tm_year, &ts.tm_mon, &ts.tm_mday, &ts.tm_hour, &ts.tm_min, &ts.tm_sec);
    ts.tm_year -= 1900;
    return mktime(&ts);
};

/**
 * @todo@b code: returning empty string
 * @param value
 * @return 
 */
template <>
inline std::string toString <IntervalEvent>(const IntervalEvent& value) {
    return std::string("");
};

/**
 * Generic conversion from string to array representation
 * @param buffer   input string
 * @param size     output value only: array size
 * @return array of values
 */
template<class T>
inline T* deserializeA(char *buffer, int& size) {
    T *ret  = NULL;
    std::string valStr   = std::string(buffer);     
    
    if ((valStr.find('[', 0) == 0) && (valStr.find(']', 0) == valStr.length()-1)) {
        size_t leftPos  = 1;
        size_t nextPos  = 1;
        size            = 1;
        leftPos         = valStr.find(',', 1);
        while ( leftPos != std::string::npos) {
            size++;
            leftPos     = valStr.find(',', leftPos+1);
        }
        
        ret         = new T[size];
        leftPos     = 1;
        for (int i = 0; i < size; i++) {
            nextPos = valStr.find_first_of(",]", leftPos+1);
            std::stringstream(valStr.substr(leftPos, nextPos-leftPos)) >> ret[i];
            leftPos     = nextPos + 1;
        }
        
    }
    else {
        ret     = new T[1];
        size    = 1;
        std::stringstream(valStr) >> ret[0];
    }

    return ret;
}

/**
 * Generic conversion from string to vector representation
 * @param buffer   input string
 * @return vector of values
 */
template<class T>
inline std::vector<T>* deserializeV(char *buffer) {
    std::vector<T> *ret  = NULL;
    std::string valStr   = std::string(buffer);

    if ((valStr.find('[', 0) == 0) && (valStr.find(']', 0) == valStr.length()-1)) {
        size_t leftPos  = 1;
        size_t nextPos  = 1;
        int size        = 1;
        leftPos         = valStr.find(',', 1);
        while ( leftPos != std::string::npos) {
            size++;
            leftPos     = valStr.find(',', leftPos+1);
        }

        ret         = new std::vector<T>;
        leftPos     = 1;
        for (int i = 0; i < size; i++) {
            T val;
            nextPos = valStr.find_first_of(",]", leftPos+1);
            std::stringstream(valStr.substr(leftPos, nextPos-leftPos)) >> val;
            ret->push_back(val);
            leftPos     = nextPos + 1;
        }

    }
    else {
        T val;
        ret = new std::vector<T>;        
        std::stringstream(valStr) >> val;
        ret->push_back(val);
    }

    return ret;
}
}

#endif	/* VTAPI_SERIALIZE_H */

