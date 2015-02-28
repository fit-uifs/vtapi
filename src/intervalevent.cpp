
#include <common/vtapi_global.h>
#include <data/vtapi_intervalevent.h>

IntervalEvent::IntervalEvent() :
group_id(-1),class_id(-1),is_root(false),score(-1.0),
user_data_size(0),user_data(NULL)
{
    memset(&region, 0, sizeof(region));
}

IntervalEvent::IntervalEvent(const IntervalEvent& orig) :
group_id(orig.group_id),class_id(orig.class_id),is_root(orig.is_root),score(orig.score)
{
    SetUserData(orig.user_data, orig.user_data_size);
}

IntervalEvent::~IntervalEvent() {
    if (user_data) free(user_data);
}

void IntervalEvent::SetUserData(void *data, size_t size) {
    if (user_data) {
        free(user_data);
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
