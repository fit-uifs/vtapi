// VTServer application - video statistics
// by ifroml[at]fit.vutbr.cz
//
// Calculate how much video is covered by evenets (trajectories etc.)

#include "sequencestats.h"
#include <cstring>

namespace vtserver {


SequenceStats::SequenceStats(unsigned int video_length)
    : _sequence_length(video_length), _count_root(0), _count_all(0)
{
    size_t bitmap_size = (video_length + 7) >> 3;
    _bitmap.resize(bitmap_size, '\0');
}

void SequenceStats::processEvent(unsigned int t1_frame, unsigned int t2_frame, const vtapi::IntervalEvent &event)
{
    // add to counts
    _count_all++;
    if (event.is_root) _count_root++;

    // get correct values (0..vidlength-1)
    int t1 = t1_frame - 1;  // start time
    int t2 = t2_frame - 1;    // end time
    if (t1 < 0) t1 = 0;
    if (t2 > _sequence_length - 1) t2 = _sequence_length - 1;
    if (t2 < t1) t2 = t1;

    // helpful index values to bitmap
    int x1 = t1 >> 3;                   // first byte in map
    int x2 = t2 >> 3;                   // last byte in map
    int y1 = 7 & t1;                    // start bit in first byte
    int y2 = 7 & t2;                    // end bit in last byte
    int bits = t2 - t1 + 1;             // total bits to set

    // fill first byte
    if (y1 > 0 && bits) {
        for (int y = 7-y1; (y >= 0) && bits; y--, bits--)
            _bitmap[x1] |= (1 << y);
        x1++;
    }

    // fill last byte
    if (bits) {
        for (int y = 7; (y >= 7-y2) && bits; y--, bits--)
            _bitmap[x2] |= (1 << y);
        x2--;
    }

    // fill stuff in between
    if (bits > 0) memset(&_bitmap[x1], 0xFF, x2-x1+1);
}

double SequenceStats::calculateCoverage()
{
    if (_sequence_length == 0) return 0.0;

    // analyze bitmap for coverage
    int x = 0, y = 7, total = 0;;
    for (int i = 0; i < _sequence_length; i++) {
        if ((_bitmap[x]) & (1 << y)) total++;
        if (--y < 0) {
            x++;
            y = 7;
        }
    }

    return static_cast<double>(total)/_sequence_length;
}



}
