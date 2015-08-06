/**
 * @file
 * @brief   Definition of serialization and deserialization functions
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <ctime>
#include <string>
#include <sstream>
#include <vector>
#include "vtapi_config.h"
#include "vtapi_misc.h"
#include "../data/vtapi_intervalevent.h"
#include "../data/vtapi_processstate.h"

namespace vtapi {
    
/**
 * @brief A generic function to convert any numeric type to string
 * (any numeric type, e.g. int, float, double, etc.)
 * @param value   numeric value
 * @return string containing the numeric value
 */
template <class T>
inline std::string toString(const T& value)
{
    std::ostringstream ostr;
    ostr << value;
    
    return ostr.str();
};

/**
 * @brief A generic function to convert any array of supported types to string
 * @param values   array of numeric values
 * @param size input array size
 * @param limit limit of array elements to serialize
 * @return string containing the serialized result
 */
template <class T>
inline std::string toString(const T* values, const int size, const int limit)
{
    std::string str;
    int lim = (limit && limit < size) ? limit : size;
    str += '[';
    for (int i = 0; i < lim; i++) {
        str += toString(values[i]);
        if (i < lim - 1)  str += ',';
    }
    str += ']';
    
    return str;
}

/**
 * @brief A generic function to convert any vector of supported types to string
 * @param values vector of values
 * @param limit limit array of elements to serialize
 * @return string containing the serialized result
 */
template <class T>
inline std::string toString(const std::vector<T>& values, const int limit)
{
    return toString(values.data(), (int) values.size(), limit);
}

// specialized toString

template <>
inline std::string toString< std::vector<int> > (const std::vector<int>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString< std::vector<double> > (const std::vector<double>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString <std::string>(const std::string& value)
{
    return value;
};

template <>
inline std::string toString <void>(const void* data, const int data_size, const int limit)
{
    return base64_encode(data, data_size);
}

template <>
inline std::string toString <time_t>(const time_t& value)
{
    struct tm *ts = gmtime(&value);
    ts->tm_year -= 1900;

    char buff[20];
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", ts);

    return buff;
};

template <>
inline std::string toString <IntervalEvent::point>(const IntervalEvent::point& value)
{
    std::ostringstream ostr;
    ostr << '(' << value.x << ',' << value.y << ')';

    return ostr.str();
};

template <>
inline std::string toString <IntervalEvent::box>(const IntervalEvent::box& value)
{
    std::string str;
    str = '(' + toString(value.high) + ',' + toString(value.low) + ')';

    return str;
}

template <>
inline std::string toString <IntervalEvent>(const IntervalEvent& value)
{
    std::ostringstream ostr;
    ostr << '(' << value.group_id << ',' << value.class_id << ',' << value.is_root << ','
    << toString(value.region) << ',' << value.score << ','
    << toString(value.user_data, value.user_data_size, 0) << ')';

    return ostr.str();
};

#if VTAPI_HAVE_POSTGRESQL
template <>
inline std::string toString <PGpoint>(const PGpoint& value)
{
    return toString<IntervalEvent::point>((const IntervalEvent::point&)value);
};

template <>
inline std::string toString <PGbox>(const PGbox& value)
{
    return toString<IntervalEvent::box>((const IntervalEvent::box&)value);
};
#endif

template <>
inline std::string toString <ProcessState>(const ProcessState& value)
{
    std::ostringstream ostr;
    ostr << '(' << ProcessState::toStatusString(value.status) << ','
        << value.progress << ',' << value.currentItem << ','
        << value.lastError << ')';

    return ostr.str();
};


#if VTAPI_HAVE_OPENCV
template <typename T>
inline std::string toStringCvMat(const cv::Mat_<T>& value)
{
    std::ostringstream ostr;
    
    int *dims = new int[value.dims];
    if (dims) {
        memset(dims, 0, value.dims * sizeof(int));
        
        ostr << '[';
        int cur = 0;
        for (cv::MatConstIterator_<T> it = value.begin(); it != value.end(); it++) {
            while (cur < value.dims - 1) {
                cur++;
                ostr << '[';
            }

            ostr << (*it);
            
            while (++(dims[cur]) == value.size[cur]) {
                dims[cur--] = 0;
                if (cur < 0) break;
                ostr << ']';
            }
            if (cur < 0) break;
            ostr << ',';
        }
        ostr << ']';
        
        delete[] dims;
    }
    
    return ostr.str();
}

template <>
inline std::string toString <cv::Mat>(const cv::Mat& value)
{
    switch(value.type())
    {
        case CV_8U:
        case CV_8S:
        {
            return toStringCvMat((const cv::Mat_<uchar> &)value);
        }
        case CV_16U:
        {
            return toStringCvMat((const cv::Mat_<unsigned short> &)value);
        }
        case CV_16S:
        {
            return toStringCvMat((const cv::Mat_<short> &)value);
        }
        case CV_32S:
        {
            return toStringCvMat((const cv::Mat_<int> &)value);
        }
        case CV_32F:
        {
            return toStringCvMat((const cv::Mat_<float> &)value);
        }
        case CV_64F:
        {
            return toStringCvMat((const cv::Mat_<double> &)value);
        }
    default:
        {
            return "";
        }
    }
};
#endif


/**
 * @brief Converts time string (yyyy-mm-dd hh:mm:ss) to timestamp
 * @param value   time string
 * @return timestamp
 */
inline time_t toTimestamp(const std::string& value)
{
    struct tm ts = {0};
    sscanf(value.c_str(), "%d-%d-%d %d:%d:%d", &ts.tm_year, &ts.tm_mon, &ts.tm_mday, &ts.tm_hour, &ts.tm_min, &ts.tm_sec);
    ts.tm_year -= 1900;
    
    return mktime(&ts);
};

/**
 * @brief Generic conversion from string to array representation
 * @param buffer   input string
 * @param size     output value only: array size
 * @return array of values
 */
template<class T>
inline T* deserializeA(const char *buffer, int& size) {
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
 * @brief Generic conversion from string to vector representation
 * @param buffer   input string
 * @return vector of values
 */
template<class T>
inline std::vector<T>* deserializeV(const char *buffer) {
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
