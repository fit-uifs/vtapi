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
        
        _point(const _point &pt)
        {
            this->x = pt.x;
            this->y = pt.y;
        }
        _point(double x = 0, double y = 0)
        {
            this->x = x;
            this->y = y;
        }
    } point;
    
    typedef struct _box
    {
        point high;
        point low;
        

        _box(const _box &b)
        {
            this->high = b.high;
            this->low = b.low;
        }
        _box(const point& high, const point& low)
        {
            this->high = high;
            this->low = low;
        }
        _box(double x_high = 0, double y_high = 0, double x_low = 0, double y_low = 0)
        {
            this->high.x = x_high;
            this->high.y = y_high;
            this->low.x = x_low;
            this->low.y = y_low;
        }
    } box;

public:
    IntervalEvent();
    IntervalEvent(const IntervalEvent& orig);
    ~IntervalEvent();
    
    IntervalEvent& operator=(const IntervalEvent& orig);
    
    void SetUserData(const void *data, size_t size);

    int group_id;           /**< groups associate events together */
    int class_id;           /**< event class (user-defined) */
    bool is_root;           /**< is this event a meta-event (eg. trajectory envelope) */
    box region;             /**< event region in video */
    double score;           /**< event score (user-defined) */
    size_t user_data_size;  /**< user_data size */
    void *user_data;        /**< additional custom user-defined data */
};

} // namespace vtapi
