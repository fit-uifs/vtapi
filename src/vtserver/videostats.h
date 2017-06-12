#pragma once

#include <vtapi/data/interval.h>
#include <vector>

namespace vtserver {


class VideoStats
{
public:
    VideoStats(unsigned int video_length);

    void processEvent(unsigned int t1_frame, unsigned int t2_frame, const vtapi::IntervalEvent &event);
    
    double calculateCoverage();

    const std::vector<char> & bitmap()
    { return _bitmap; }
    unsigned int count_root()
    { return _count_root; }
    unsigned int count_all()
    { return _count_all; }

private:
    unsigned int _video_length;
    std::vector<char> _bitmap;
    unsigned int _count_root;
    unsigned int _count_all;

private:
    VideoStats() = delete;
};

}
