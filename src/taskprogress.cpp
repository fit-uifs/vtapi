
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/delete.h>
#include <vtapi/data/taskprogress.h>

using namespace std;

namespace vtapi {


TaskProgress::TaskProgress(const Commons &commons,
                           const string& taskname,
                           const string& seqname,
                           bool acquired)
    : KeyValues(commons, def_tab_tasks_seq), _acquired(acquired), _update_set(false)
{
    if (_context.dataset.empty())
        throw BadConfigurationException("dataset not specified");

    if (!taskname.empty())
        _context.task = taskname;

    if (!seqname.empty())
        _context.sequence = seqname;

    if(!_context.task.empty())
        _select.querybuilder().whereString(def_col_tsd_taskname, _context.task);

    if (!_context.sequence.empty())
        _select.querybuilder().whereString(def_col_tsd_seqname, _context.sequence);
}


TaskProgress::TaskProgress(const Commons &commons,
                           const string& taskname,
                           const vector<string> &seqnames)
    : KeyValues(commons, def_tab_tasks_seq), _acquired(false), _update_set(false)
{
    if (_context.dataset.empty())
        throw BadConfigurationException("dataset not specified");

    if (!taskname.empty())
        _context.task = taskname;

    if(!_context.task.empty())
        _select.querybuilder().whereString(def_col_tsd_taskname, _context.task);

    _select.querybuilder().whereStringVector(def_col_tsd_seqname, seqnames);
}

TaskProgress::~TaskProgress()
{
    if (_acquired && !_update_set) {
        Delete d(*this, def_tab_tasks_seq);
        d.querybuilder().whereString(def_col_tsd_taskname, _context.task);
        d.querybuilder().whereString(def_col_tsd_seqname, _context.sequence);
        d.execute();
    }
}

bool TaskProgress::next()
{
    if (KeyValues::next()) {
        _context.task = this->getTaskName();
        _context.sequence = this->getSequenceName();
        return true;
    }
    else {
        return false;
    }
}

string TaskProgress::getTaskName() const
{
    return this->getString(def_col_tsd_taskname);
}

Task *TaskProgress::getTask() const
{
    return new Task(*this, this->getTaskName());
}

int TaskProgress::getProcessId() const
{
    return this->getInt(def_col_tsd_prsid);
}


Process *TaskProgress::getProcess() const
{
    return new Process(*this, this->getProcessId());
}

string TaskProgress::getSequenceName() const
{
    return this->getString(def_col_tsd_seqname);
}


Sequence *TaskProgress::getSequence() const
{
    return new Sequence(*this, this->getSequenceName());
}

bool TaskProgress::getIsDone() const
{
    return this->getBool(def_col_tsd_isdone);
}

chrono::system_clock::time_point TaskProgress::getStartedTime() const
{
    return this->getTimestamp(def_col_tsd_started);
}

chrono::system_clock::time_point TaskProgress::getFinishedTime() const
{
    return this->getTimestamp(def_col_tsd_finished);
}

bool TaskProgress::updateIsDone(bool finished)
{
    if (!_acquired) {
        return false;
    }
    else {
        chrono::system_clock::time_point now = chrono::system_clock::now();

        return _update_set =
                (this->updateBool(def_col_tsd_isdone, finished) &&
                this->updateTimestamp(def_col_tsd_finished, now) &&
                this->updateExecute());
    }
}

bool TaskProgress::preUpdate()
{
    return update().querybuilder().whereString(def_col_tsd_taskname, _context.task) &&
            update().querybuilder().whereString(def_col_tsd_seqname, _context.sequence);
}

}
