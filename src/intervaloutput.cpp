
#include <exception>
#include <vtapi/common/global.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/intervaloutput.h>

using namespace std;

namespace vtapi {


IntervalOutput::IntervalOutput(const Commons &commons,
                               const string &sequence,
                               const string &output)
    : Commons(commons, false)
{
    if (!sequence.empty())
        context().sequence = sequence;
    if (!output.empty())
        context().selection = output;

    if (context().dataset.empty() ||
        context().task.empty() ||
        context().sequence.empty() ||
        context().selection.empty())
    {
        throw exception();
    }
}


IntervalOutput::~IntervalOutput()
{
    discard();
}

bool IntervalOutput::newInterval(int t1, int t2)
{
    bool ret = true;

    Insert *insert = new Insert(*this);
    ret &= insert->keyString(def_col_int_taskname, context().task);
    ret &= insert->keyString(def_col_int_seqname, context().sequence);
    ret &= insert->keyInt(def_col_int_t1, t1);
    ret &= insert->keyInt(def_col_int_t2, t2);

    if (ret)
        _inserts.push_back(insert);
    else
        vt_destruct(insert);

    return ret;
}

bool IntervalOutput::setString(const string &key, const string &value)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyString(key, value);
}

bool IntervalOutput::setStringArray(const string &key, string *values, int size)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyStringA(key, values, size);
}

bool IntervalOutput::setBool(const string &key, bool value)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyBool(key, value);
}

bool IntervalOutput::setInt(const string &key, int value)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyInt(key, value);
}

bool IntervalOutput::setIntArray(const string &key, int *values, int size)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyIntA(key, values, size);
}

bool IntervalOutput::setFloat(const string &key, double value)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyFloat8(key, value);
}

bool IntervalOutput::setFloatArray(const string &key, double *values, int size)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyFloat8A(key, values, size);
}

bool IntervalOutput::setTimestamp(const string &key, const time_t value)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyTimestamp(key, value);
}

bool IntervalOutput::setCvMat(const string &key, const cv::Mat &value)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyCvMat(key, value);
}

bool IntervalOutput::setIntervalEvent(const string &key, const IntervalEvent &value)
{
    if (_inserts.empty())
        return false;
    else
        return _inserts.back()->keyIntervalEvent(key, value);
}

bool IntervalOutput::commit()
{
    bool ret = true;
    bool transaction = false;

    if (QueryBeginTransaction(*this).execute())
        transaction = true;

    for (auto insert : _inserts) {
        if (!(ret = insert->execute())) break;
    }

    if (transaction) {
        if (ret)
            QueryCommitTransaction(*this).execute();
        else
            QueryRollbackTransaction(*this).execute();
    }

    if (ret) discard();

    return ret;
}

void IntervalOutput::discard()
{
    for (auto insert : _inserts)
        delete insert;
    _inserts.clear();
}


}
