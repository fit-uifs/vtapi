
#include <exception>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/delete.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/intervaloutput.h>

using namespace std;

namespace vtapi {


IntervalOutput::IntervalOutput(const Commons &commons,
                               const string &sequence,
                               const string &output,
                               unsigned int cache_limit)
    : Commons(commons, false), _cache_limit(cache_limit)
{
    if (!sequence.empty())
        _context.sequence = sequence;
    if (!output.empty())
        _context.selection = output;

    if (_context.dataset.empty() ||
        _context.task.empty() ||
        _context.sequence.empty() ||
        _context.selection.empty())
    {
        string error = "cannot create interval output without dataset, task, sequence or output table";
        throw BadConfigurationException(error);
    }
}

bool IntervalOutput::newInterval(int t1, int t2)
{
    bool ret = true;

    if (_cache_limit > 0 && _inserts.size() >= _cache_limit)
        ret = this->commit();

    if (ret) {
        _inserts.push_back(std::shared_ptr<Insert>(new Insert(*this, _context.selection)));
        Insert & i = last_insert();

        ret &= i.querybuilder().keyString(def_col_int_taskname, _context.task);
        ret &= i.querybuilder().keyString(def_col_int_seqname, _context.sequence);
        ret &= i.querybuilder().keyInt(def_col_int_t1, t1);
        ret &= i.querybuilder().keyInt(def_col_int_t2, t2);

        if (!ret) _inserts.pop_back();
    }

    return ret;
}

Insert & IntervalOutput::last_insert()
{
    if (_inserts.empty())
        throw RuntimeException("Invalid interval INSERT: call newInterval() first");

    return *_inserts.back();
}

bool IntervalOutput::commit()
{
    bool ret = true;

    if (_inserts.size() > 0) {
        bool transaction = QueryBeginTransaction(*this).execute();

        for (auto insert : _inserts) {
            if (!(ret = insert->execute())) break;
        }

        if (transaction) {
            if (ret)
                QueryCommitTransaction(*this).execute();
            else
                QueryRollbackTransaction(*this).execute();
        }

        if (ret) discard(true);
    }

    return ret;
}

bool IntervalOutput::discard(bool only_cached)
{
    bool ret = true;

    _inserts.clear();

    if (!only_cached) {
        Delete d(*this, _context.selection);
        ret &= d.querybuilder().whereString(def_col_int_taskname, _context.task);
        ret &= d.querybuilder().whereString(def_col_int_seqname, _context.sequence);
        if (ret) ret &= d.execute();
    }

    return ret;
}


}
