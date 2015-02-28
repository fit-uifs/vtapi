
#ifndef INTERVALEVENT_H
#define	INTERVALEVENT_H

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

