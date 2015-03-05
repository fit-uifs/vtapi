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
    typedef struct
    {
        double x;
        double y;
    } point;
    
    typedef struct
    {
        point high;
        point low;
    } box;

public:
    IntervalEvent();
    IntervalEvent(const IntervalEvent& orig);
    virtual ~IntervalEvent();
    
    void SetUserData(void *data, size_t size);
    
    int group_id;
    int class_id;
    bool is_root;
    box region;
    double score;
    size_t user_data_size;
    void *user_data;

};

#endif	/* INTERVALEVENT_H */

} // namespace vtapi