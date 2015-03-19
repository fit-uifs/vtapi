/**
 * @file
 * @brief   Declaration of IntervalEvent class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef INTERVALEVENT_H
#define	INTERVALEVENT_H

namespace vtapi {
    
/**
 * @brief
 * 
 * @todo @b doc: put together a few letters into brief description and class members and methods..
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class IntervalEvent {
public:
    typedef struct _point
    {
        double x;
        double y;
        
        _point()
        {
            memset(this, 0, sizeof(point));
        };
        _point(const _point &pt)
        {
            memcpy(this, &pt, sizeof(point));
        };
        _point(const double x, const double y)
        {
            this->x = x;
            this->y = y;
        };
    } point;
    
    typedef struct _box
    {
        point high;
        point low;
        
        _box()
        {
            memset(this, 0, sizeof(box));
        };
        _box(const _box &b)
        {
            memcpy(this, &b, sizeof(box));
        };  
        _box(const point high, const point low)
        {
            this->high = high;
            this->low = low;
        };
        _box(const double x_high, const double y_high, const double x_low, const double y_low)
        {
            this->high.x = x_high;
            this->high.y = y_high;
            this->low.x = x_low;
            this->low.y = y_low;
        };
    } box;

public:
    IntervalEvent();
    IntervalEvent(const IntervalEvent& orig);
    virtual ~IntervalEvent();
    
    void SetUserData(const void *data, size_t size);
    
    int group_id;
    int class_id;
    bool is_root;
    box region;
    double score;
    size_t user_data_size;
    void *user_data;

};

} // namespace vtapi

#endif	/* INTERVALEVENT_H */
