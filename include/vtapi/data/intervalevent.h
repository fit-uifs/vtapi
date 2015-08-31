/**
 * @file
 * @brief   Declaration of IntervalEvent class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <vector>

namespace vtapi {


/**
 * @brief
 * 
 * Class representing universal event in video interval
 * 
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class IntervalEvent
{
public:
    /**
     * @brief Simple 2D point
     */
    class Point
    {
    public:
        double x;
        double y;
        
        Point()
            : x(0), y(0) {}
        Point(double x, double y)
            : x(x), y(y) {}
    };
    
    /**
     * @brief Rectangle region identified by two 2D points
     */
    class Box
    {
    public:
        Point high;
        Point low;

        Box() {}
        Box(const Point& high, const Point& low)
            : high(high), low(low) {}
        Box(double x_high, double y_high, double x_low, double y_low)
            : high(x_high, y_high), low (x_low, y_low) {}
    };


    int group_id;           /**< groups associate events together */
    int class_id;           /**< event class (user-defined) */
    bool is_root;           /**< is this event a meta-event (eg. trajectory envelope) */
    double score;           /**< event score (user-defined) */
    Box region;             /**< event region in video */
    std::vector<char> user_data;    /**< additional custom user-defined data */


    IntervalEvent() : group_id(-1), class_id(-1), is_root(false), score(-1.0) {}
};

typedef IntervalEvent::Point Point;
typedef IntervalEvent::Box Box;


} // namespace vtapi
