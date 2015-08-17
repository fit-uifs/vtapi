
#include <cstring>
#include <vtapi/common/global.h>
#include <vtapi/data/intervalevent.h>

namespace vtapi {


IntervalEvent::IntervalEvent()
    : group_id(-1), class_id(-1), is_root(false),
    score(-1.0), user_data_size(0), user_data(NULL) { }

IntervalEvent::IntervalEvent(const IntervalEvent& orig)
    : group_id(orig.group_id), class_id(orig.class_id), is_root(orig.is_root),
    score(orig.score), region(orig.region), user_data_size(0), user_data(NULL)
{
    SetUserData(orig.user_data, orig.user_data_size);
}

IntervalEvent::~IntervalEvent()
{
    if (user_data) free(user_data);
}

IntervalEvent& IntervalEvent::operator=(const IntervalEvent& orig)
{
    group_id = orig.group_id;
    class_id = orig.class_id;
    is_root = orig.is_root;
    score = orig.score;
    region = orig.region;

    SetUserData(orig.user_data, orig.user_data_size);
}

void IntervalEvent::SetUserData(const void *data, size_t size)
{
    if (user_data) {
        free(user_data);
        user_data = NULL;
        user_data_size = 0;
    }

    if (data && size > 0) {
        user_data = malloc(size);
        if (user_data) {
            memcpy(user_data, data, size);
            user_data_size = size;
        }
    }
}


IntervalEvent::_point::_point()
{
    this->x = 0;
    this->y = 0;
}

IntervalEvent::_point::_point(double x, double y)
{
    this->x = x;
    this->y = y;
}

IntervalEvent::_box::_box()
    : high(), low()
{
}

IntervalEvent::_box::_box(const point& high, const point& low)
    : high(high), low(low)
{
}

IntervalEvent::_box::_box(double x_high, double y_high, double x_low, double y_low)
    : high(x_high, y_high), low(x_low, y_low)
{
}


}

