#pragma once

#include "../data/commons.h"
#include "../queries/insert.h"
#include "../queries/query.h"
#include <list>
#include <memory>
#include <opencv2/opencv.hpp>

namespace vtapi {


class IntervalOutput : protected Commons
{
public:
    /**
     * @brief Constructor
     * @param commons shared commons object
     * @param sequence for which sequence should intervals be created
     * @param output output table name
     * @param cache_limit maximum cached intervals before commit (0 = unlimited)
     */
    IntervalOutput(const Commons &commons,
                   const std::string &sequence,
                   const std::string &output,
                   unsigned int cache_limit = 500);

    /**
     * @brief Creates new current interval into cache for later commit
     * @param t1 interval start frame
     * @param t2 interval end frame
     * @return success
     */
    bool newInterval(int t1, int t2);

    /**
     * @brief Create custom query
     * @return query object
     **/
    Query *createCustomQuery(std::string sql);

    /**
     * @brief Commits all cached intervals added with newInterval()
     * @return success
     */
    bool commit();

    /**
     * @brief Discards all intervals added with newInterval()
     * @param only_cached true = discard cached intervals,
     * false = also delete previously commited intervals
     * @return successful delete on only_cached == false, true otherwise
     */
    bool discard(bool only_cached);

    // value setters - call these after calling newInterval()
    // and before commit() to set interval parameters

    inline bool setBool(const std::string &key, bool value)
    { last_insert().querybuilder().keyBool(key, value); }

    inline bool setChar(const std::string &key, char value)
    { last_insert().querybuilder().keyChar(key,value); }

    inline bool setString(const std::string &key, const std::string &value)
    { last_insert().querybuilder().keyString(key,value); }

    inline bool setStringVector(const std::string &key, const std::vector<std::string> &values)
    { last_insert().querybuilder().keyStringVector(key,values); }

    inline bool setInt(const std::string &key, int value)
    { last_insert().querybuilder().keyInt(key,value); }

    inline bool setIntVector(const std::string &key, const std::vector<int> &values)
    { last_insert().querybuilder().keyIntVector(key,values); }

    inline bool setInt8(const std::string &key, long long value)
    { last_insert().querybuilder().keyInt8(key,value); }

    inline bool setInt8Vector(const std::string &key, const std::vector<long long> &values)
    { last_insert().querybuilder().keyInt8Vector(key,values); }

    inline bool setFloat(const std::string &key, float value)
    { last_insert().querybuilder().keyFloat(key,value); }

    inline bool setFloatVector(const std::string &key, const std::vector<float> &values)
    { last_insert().querybuilder().keyFloatVector(key,values); }

    inline bool setFloat8(const std::string &key, double value)
    { last_insert().querybuilder().keyFloat8(key,value); }

    inline bool setFloat8Vector(const std::string &key, const std::vector<double> &values)
    { last_insert().querybuilder().keyFloat8Vector(key,values); }

    inline bool setTimestamp(const std::string &key, const std::chrono::system_clock::time_point &value)
    { last_insert().querybuilder().keyTimestamp(key,value); }

    inline bool setCvMat(const std::string &key, const cv::Mat &value)
    { last_insert().querybuilder().keyCvMat(key,value); }

    inline bool setPoint(const std::string &key, Point value)
    { last_insert().querybuilder().keyPoint(key,value); }

    inline bool setPointVector(const std::string &key, const std::vector<Point> &values)
    { last_insert().querybuilder().keyPointVector(key,values); }

    inline bool setIntervalEvent(const std::string &key, const IntervalEvent &value)
    { last_insert().querybuilder().keyIntervalEvent(key,value); }

    inline bool setEdfDescriptor(const std::string &key, const EyedeaEdfDescriptor &value)
    { last_insert().querybuilder().keyEdfDescriptor(key, value); }

    inline bool setBlob(const std::string &key, const std::vector<char> &data)
    { last_insert().querybuilder().keyBlob(key,data); }

private:
    std::list< std::shared_ptr<Insert> > _inserts;
    unsigned int _cache_limit;

    Insert & last_insert();

    IntervalOutput() = delete;
    IntervalOutput(const IntervalOutput &) = delete;
    IntervalOutput & operator=(const IntervalOutput &) = delete;
};

}
