/**
 * @file
 * @brief   Declaration of EventFilter class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */


#pragma once

#include "intervalevent.h"
#include <chrono>

namespace vtapi {


class EventFilter
{
public:
    struct Duration
    {
        std::chrono::microseconds _low;
        std::chrono::microseconds _high;

        Duration()
            : _low(-1), _high(-1) {}
        Duration(const std::chrono::microseconds & low,
                 const std::chrono::microseconds & high)
            : _low(low), _high(high) {}
    };

    struct TimeRange
    {
        std::chrono::system_clock::time_point _low;
        std::chrono::system_clock::time_point _high;

        TimeRange()
            : _low(std::chrono::system_clock::duration::zero()),
              _high(std::chrono::system_clock::duration::zero()) {}
        TimeRange(const std::chrono::system_clock::time_point & low,
                  const std::chrono::system_clock::time_point & high)
            : _low(low), _high(high) {}
    };

    struct DayTimeRange
    {
        std::chrono::microseconds _low;
        std::chrono::microseconds _high;

        DayTimeRange()
            : _low(-1), _high(-1) {}
        DayTimeRange(const std::chrono::microseconds & low,
                     const std::chrono::microseconds & high)
            : _low(low), _high(high) {}
    };

public:
    EventFilter()
        : _region(IntervalEvent::Point(-1,-1), IntervalEvent::Point(-1,-1)) {}

    bool hasDurationFilter() const
    { return _duration._low.count() >= 0 && _duration._high.count() >= 0; }

    bool hasTimeRangeFilter() const
    {
        return _timerange._low.time_since_epoch() != std::chrono::system_clock::duration::zero() &&
                _timerange._high.time_since_epoch() != std::chrono::system_clock::duration::zero();
    }

    bool hasDayTimeRangeFilter() const
    { return _daytimerange._low.count() >= 0 && _daytimerange._high.count() >= 0; }

    bool hasRegionFilter() const
    {
        return _region.high.x >= 0 && _region.high.y >= 0 &&
                _region.low.x >= 0 && _region.low.y >= 0;
    }

    Duration getDurationFilter() const
    { return _duration; }

    TimeRange getTimeRangeFilter() const
    { return _timerange; }

    DayTimeRange getDayTimeRangeFilter() const
    { return _daytimerange; }

    IntervalEvent::Box getRegionFilter() const
    { return _region; }

    void setDurationFilter(const Duration & filter)
    { _duration = filter; }

    void setTimeRangeFilter(const TimeRange & filter)
    { _timerange = filter; }

    void setDayTimeRangeFilter(const DayTimeRange & filter)
    { _daytimerange = filter; }

    void setRegionFilter(const IntervalEvent::Box & filter)
    { _region = filter; }

private:
    Duration _duration;
    TimeRange _timerange;
    DayTimeRange _daytimerange;
    IntervalEvent::Box _region;
};

}
