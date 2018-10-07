/**
 * @file
 * @brief   Methods of Task class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <exception>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/insert.h>
#include <vtapi/queries/delete.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/task.h>


using namespace std;

namespace vtapi {


Task::Task(const Task &copy)
    : Task(dynamic_cast<const Commons&>(copy))
{
}

Task::Task(const Commons& commons, const string& name)
    : KeyValues(commons, def_tab_tasks)
{
    if (_context.dataset.empty())
        throw BadConfigurationException("dataset not specified");

    if (!name.empty())
        _context.task = name;

    // normally SELECT selects all columns implicitly
    // but we can't handle regclass => specify explicitly
    _select.from(def_tab_tasks, def_col_task_name);
    _select.from(def_tab_tasks, def_col_task_mtname);
    _select.from(def_tab_tasks, def_col_task_params);
    _select.from(def_tab_tasks, def_col_task_outputs + "::text");
    _select.from(def_tab_tasks, def_col_task_prsid);
    _select.from(def_tab_tasks, def_col_task_created);

    _select.setOrderBy(def_col_task_name);

    if (!_context.task.empty()) {
        _select.querybuilder().whereString(def_col_task_name, _context.task);
    }
    else {
        if (!_context.method.empty())
            _select.querybuilder().whereString(def_col_task_mtname, _context.method);
    }
}

Task::Task(const Commons& commons, const vector<string>& names)
    : KeyValues(commons, def_tab_tasks)
{
    if (_context.dataset.empty())
        throw BadConfigurationException("dataset not specified");

    // normally SELECT selects all columns implicitly
    // but we can't handle regclass => specify explicitly
    _select.from(def_tab_tasks, def_col_task_name);
    _select.from(def_tab_tasks, def_col_task_mtname);
    _select.from(def_tab_tasks, def_col_task_params);
    _select.from(def_tab_tasks, def_col_task_outputs + "::text");
    _select.from(def_tab_tasks, def_col_task_prsid);
    _select.from(def_tab_tasks, def_col_task_created);

    _select.setOrderBy(def_col_task_name);
    _select.querybuilder().whereStringVector(def_col_task_name, names);

    _context.method.clear();
}

bool Task::next()
{
    if (KeyValues::next()) {
        _context.task = this->getName();
        return true;
    }
    else {
        return false;
    }
}

Dataset *Task::getParentDataset() const
{
    Dataset *d = new Dataset(*this);
    if (d->next()) {
        return d;
    }
    else {
        delete d;
        return NULL;
    }
}

string Task::getParentMethodName() const
{
    if (!_context.method.empty())
        return _context.method;
    else
        return this->getString(def_col_task_mtname);
}

Method *Task::getParentMethod() const
{
    string mtname = this->getParentMethodName();

    if (!mtname.empty()) {
        Method *m = new Method(*this, mtname);
        if (m->next()) {
            return m;
        }
        else {
            delete m;
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

Task *Task::loadPrerequisiteTasks() const
{
    KeyValues kv(*this, def_tab_tasks_prereq);
    kv._select.querybuilder().whereString(def_col_tprq_taskname, _context.task);

    vector<string> tasknames;
    while(kv.next()) {
        tasknames.push_back(kv.getString(def_col_tprq_taskprereq));
    }

    return new Task(*this, tasknames);
}

string Task::getOutputDataTable() const
{
    return this->getString(def_col_task_outputs);
}

TaskProgress *Task::loadTaskProgress() const
{
    return new TaskProgress(*this, _context.task);
}

TaskProgress *Task::loadTaskProgress(const vector<string>& seqnames) const
{
    return new TaskProgress(*this, _context.task, seqnames);
}

Interval *Task::loadOutputData() const
{
    return new Interval(*this, this->getOutputDataTable());
}

Interval *Task::loadOutputData(std::string outputDataTable) const
{
    if (outputDataTable == std::string()) {
        throw RuntimeException("Trying to load empty outputDataTable");
    }

    return new Interval(*this, std::string(this->getParentDataset()->getName() + "." + outputDataTable), true);
}

Process* Task::loadProcesses(int id) const
{
    return (new Process(*this, id));
}

string Task::getName() const
{
    return this->getString(def_col_task_name);
}

TaskParams Task::getParams() const
{
    return TaskParams(this->getString(def_col_task_params));
}

chrono::system_clock::time_point Task::getCreatedTime() const
{
    return this->getTimestamp(def_col_task_created);
}


int Task::getProcessId() const
{
    return this->getInt(def_col_task_prsid);
}


Process* Task::getProcess() const
{
    return new Process(*this, this->getProcessId());
}


Process* Task::createProcess(const vector<string>& seqnames) const
{
    Process *p = NULL;
    int prsid = 0;
    bool retval = true;

    QueryBeginTransaction(*this).execute();

    {
        Insert insert(*this, def_tab_processes);
        retval &= insert.querybuilder().keyString(def_col_prs_taskname, _context.task);

        if (retval && (retval = insert.execute())) {
            QueryLastInsertedId q(*this);
            if (retval = q.execute()) {
                prsid = q.getLastId();
                Method *mt = this->getParentMethod();
                if (mt && ! mt->isProgressBasedOnSeq()) {
                    Update update(*this, def_tab_tasks);
                    retval &= update.querybuilder().keyInt(def_col_task_prsid, prsid);
                    retval &= update.querybuilder().whereString(def_col_task_name, this->getName());
                    retval &= update.execute();
                }
                else {
                    for (auto & item : seqnames) {
                        Insert insert2(*this, def_tab_processes_seq);
                        retval &= insert2.querybuilder().keyInt(def_col_prss_prsid, prsid);
                        retval &= insert2.querybuilder().keyString(def_col_prss_seqname, item);
                        if (retval && (retval = insert2.execute())) {
                        }
                        else {
                            break;
                        }
                    }
                }

                delete mt;
            }
        }
    }

    if (retval) {
        QueryCommitTransaction(*this).execute();

        p = loadProcesses(prsid);
        if (!p->next()) vt_destruct(p);
    }
    else {
        QueryRollbackTransaction(*this).execute();
    }

    return p;
}

IntervalOutput *Task::createIntervalOutput(const string &seqname) const
{
    return new IntervalOutput(*this, seqname, this->getOutputDataTable());
}

bool Task::deleteOutputData(const string &seqname) const
{
    bool ret = true;
    Delete d(*this, this->getOutputDataTable());
    ret &= d.querybuilder().whereString(def_col_int_taskname, this->getName());
    if (!seqname.empty())
        ret &= d.querybuilder().whereString(def_col_int_seqname, seqname);

    return ret && d.execute();
}

bool Task::deleteOutputData(const vector<string> &seqnames) const
{
    bool ret = true;
    Delete d(*this, this->getOutputDataTable());
    ret &= d.querybuilder().whereString(def_col_int_taskname, this->getName());
    ret &= d.querybuilder().whereStringVector(def_col_int_seqname, seqnames);

    return ret && d.execute();
}

bool Task::deleteOutputData(int intervalID) const
{
    bool ret = true;
    Delete d(*this, this->getOutputDataTable());
    ret &= d.querybuilder().whereInt(def_col_int_id, intervalID);

    return ret && d.execute();
}

bool Task::deleteOutputData(const std::vector<int> & intervalIDs) const
{
    bool ret = true;
    Delete d(*this, this->getOutputDataTable());
    ret &= d.querybuilder().whereIntVector(def_col_int_id, intervalIDs);

    return ret && d.execute();
}

string Task::constructName(const string &mtname, const TaskParams &params)
{
    string input = mtname + params.serialize();

    hash<string> hash_fn;
    stringstream ss;
    ss << hex << hash_fn(input);

    return ss.str();
}

bool Task::preUpdate()
{
    return update().querybuilder().whereString(def_col_task_name, _context.task);
}

}
