/**
 * @file
 * @brief   Definition of serialization functions
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "../data/intervalevent.h"
#include "../data/processstate.h"
#include "../data/eventfilter.h"
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <opencv2/opencv.hpp>

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
}

/**
 * @brief A generic function to convert any vector of supported types to string
 * @param values vector of values
 * @param limit limit array of elements to serialize
 * @return string containing the serialized result
 */
template <class T>
inline std::string toString(const std::vector<T>& values, int limit)
{
    std::string str;
    int lim = (limit && limit < values.size()) ? limit : values.size();
    str += '[';
    for (int i = 0; i < lim; i++) {
        str += toString<T>(values[i]);
        if (i < lim - 1)  str += ',';
    }
    str += ']';

    return str;
}

// specialized toString

template <>
inline std::string toString< std::vector<std::string> > (const std::vector<std::string>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString< std::vector<int> > (const std::vector<int>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString< std::vector<long long> > (const std::vector<long long>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString< std::vector<float> > (const std::vector<float>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString< std::vector<double> > (const std::vector<double>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString< std::vector<Point> > (const std::vector<Point>& values)
{
    return toString(values, 0);
}

template <>
inline std::string toString < std::vector<char> >(const std::vector<char>& values)
{
    return "";

    //TODO: serialize binary data
    //return base64_encode(data, data_size);
}

template <>
inline std::string toString <std::string>(const std::string& value)
{
    return value;
}

template <>
inline std::string toString <std::chrono::system_clock::time_point>(const std::chrono::system_clock::time_point& value)
{
    std::time_t tmp = std::chrono::system_clock::to_time_t(value);
    std::tm ts = *std::gmtime(&tmp);

    auto secs = std::chrono::duration_cast<std::chrono::seconds>(value.time_since_epoch());
    auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(value.time_since_epoch());

    std::ostringstream oss;
    oss << std::setfill('0') <<
           std::setw(4) << ts.tm_year + 1900 << '-' <<
           std::setw(2) << ts.tm_mon + 1 << '-' <<
           std::setw(2) << ts.tm_mday << ' ' <<
           std::setw(2) << ts.tm_hour << ':' <<
           std::setw(2) << ts.tm_min << ':' <<
           std::setw(2) << ts.tm_sec << '.' <<
           std::chrono::duration_cast<std::chrono::microseconds>(usecs - secs).count();

    return oss.str();
}

template <>
inline std::string toString <std::chrono::microseconds>(const std::chrono::microseconds& value)
{
    std::chrono::hours hours = std::chrono::duration_cast<std::chrono::hours>(value);
    std::chrono::minutes mins = std::chrono::duration_cast<std::chrono::minutes>(value) % 60;
    std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(value) % 60;

    std::ostringstream oss;
    oss << std::setfill('0') <<
           std::setw(2) << hours.count() << ':' <<
           std::setw(2) << mins.count() << ':' <<
           std::setw(2) << secs.count() << '.' <<
           value.count() % 1000;

    return oss.str();
}

template <>
inline std::string toString <IntervalEvent::Point>(const IntervalEvent::Point& value)
{
    std::ostringstream ostr;
    ostr << '(' << value.x << ',' << value.y << ')';

    return ostr.str();
}

template <>
inline std::string toString <IntervalEvent::Box>(const IntervalEvent::Box& value)
{
    std::string str;
    str = '(' + toString(value.high) + ',' + toString(value.low) + ')';

    return str;
}

template <>
inline std::string toString <IntervalEvent>(const IntervalEvent& value)
{
    std::ostringstream ostr;
    ostr << '(' <<
            value.group_id << ',' <<
            value.class_id << ',' <<
            value.is_root << ',' <<
            toString(value.region) << ',' <<
            value.score << ',' <<
            toString(value.user_data) << ')';

    return ostr.str();
}

template <>
inline std::string toString <ProcessState>(const ProcessState& value)
{
    std::ostringstream ostr;
    ostr << '(' <<
            ProcessState::toStatusString(value.status) << ',' <<
            value.progress << ',' <<
            value.current_item << ',' <<
            value.last_error << ')';

    return ostr.str();
}

template <>
inline std::string toString <EventFilter>(const EventFilter& value)
{
    //(0,5,,2015-04-01 04:06:00,,,"(0,0),(0.1,0.8)")
    std::ostringstream ostr;
    ostr << '(';

    if (value.hasDurationFilter()) {
        EventFilter::Duration filter = value.getDurationFilter();
        ostr << static_cast<double>(filter._low.count()) / 1000000.0 << ',' <<
                static_cast<double>(filter._high.count()) / 1000000.0 << ',';
    }
    else {
        ostr << ",,";
    }
    if (value.hasTimeRangeFilter()) {
        EventFilter::TimeRange filter = value.getTimeRangeFilter();
        ostr << toString(filter._low) << ',' <<
                toString(filter._high) << ',';
    }
    else {
        ostr << ",,";
    }
    if (value.hasDayTimeRangeFilter()) {
        EventFilter::DayTimeRange filter = value.getDayTimeRangeFilter();
        ostr << toString(filter._low) << ',' <<
                toString(filter._high) << ',';
    }
    else {
        ostr << ",,";
    }
    if (value.hasRegionFilter()) {
        ostr << "\"" << toString(value.getRegionFilter()) << "\"";
    }

    ostr << ')';

    return ostr.str();
}

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
}


}
