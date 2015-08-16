
#pragma once

#include <list>
#include "../data/commons.h"
#include "../queries/insert.h"

namespace vtapi {


class IntervalOutput : protected Commons
{
public:
    IntervalOutput(const Commons &commons,
                   const std::string &sequence,
                   const std::string &output);
    ~IntervalOutput();

    bool newInterval(int t1, int t2);

    bool setString(const std::string &key, const std::string &value);
    bool setStringArray(const std::string &key, std::string *values, int size);
    bool setBool(const std::string &key, bool value);
    bool setInt(const std::string &key, int value);
    bool setIntArray(const std::string &key, int *values, int size);
    bool setFloat(const std::string &key, double value);
    bool setFloatArray(const std::string &key, double *values, int size);
    bool setTimestamp(const std::string &key, time_t value);
    bool setCvMat(const std::string &key, const cv::Mat &value);
    bool setIntervalEvent(const std::string &key, const IntervalEvent &value);

    bool commit();
    void discard();

private:
    std::list<Insert*> _inserts;

};

}
