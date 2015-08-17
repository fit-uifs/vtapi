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
    typedef struct _point
    {
        double x;
        double y;
        
        _point();
        _point(double x, double y);
    } point;
    
    typedef struct _box
    {
        point high;
        point low;

        _box();
        _box(const point& high, const point& low);
        _box(double x_high, double y_high, double x_low, double y_low);
    } box;


    int group_id;           /**< groups associate events together */
    int class_id;           /**< event class (user-defined) */
    bool is_root;           /**< is this event a meta-event (eg. trajectory envelope) */
    box region;             /**< event region in video */
    double score;           /**< event score (user-defined) */
    size_t user_data_size;  /**< user_data size */
    void *user_data;        /**< additional custom user-defined data */


    IntervalEvent();
    IntervalEvent(const IntervalEvent& orig);
    ~IntervalEvent();
    
    IntervalEvent& operator=(const IntervalEvent& orig);
    
    void SetUserData(const void *data, size_t size);
};

} // namespace vtapi
