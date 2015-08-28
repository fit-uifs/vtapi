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
    if (context().dataset.empty())
        throw BadConfigurationException("dataset not specified");
    
    if (!name.empty())
        context().task = name;
    
    // normally SELECT selects all columns implicitly
    // but we can't handle regclass => specify explicitly
    select().from(def_tab_tasks, def_col_task_name);
    select().from(def_tab_tasks, def_col_task_mtname);
    select().from(def_tab_tasks, def_col_task_params);
    select().from(def_tab_tasks, def_col_task_outputs + "::text");
    select().from(def_tab_tasks, def_col_task_created);

    select().setOrderBy(def_col_task_name);
    
    if (!context().task.empty()) {
        select().whereString(def_col_task_name, context().task);
    }
    else {
        if (!context().method.empty())
            select().whereString(def_col_task_mtname, context().method);
    }
}

Task::Task(const Commons& commons, const list<string>& names)
    : KeyValues(commons, def_tab_tasks)
{
    if (context().dataset.empty())
        throw BadConfigurationException("dataset not specified");
    
    // normally SELECT selects all columns implicitly
    // but we can't handle regclass => specify explicitly
    select().from(def_tab_tasks, def_col_task_name);
    select().from(def_tab_tasks, def_col_task_mtname);
    select().from(def_tab_tasks, def_col_task_params);
    select().from(def_tab_tasks, def_col_task_outputs + "::text");
    select().from(def_tab_tasks, def_col_task_created);

    select().setOrderBy(def_col_task_name);
    select().whereStringInList(def_col_task_name, names);
    
    context().method.clear();
}

bool Task::next()
{
    if (KeyValues::next()) {
        context().task = this->getName();
        return true;
    }
    else {
        return false;
    }
}

Dataset *Task::getParentDataset()
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

string Task::getParentMethodName()
{
    if (!context().method.empty())
        return context().method;
    else
        return this->getString(def_col_task_mtname);
}

Method *Task::getParentMethod()
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

Task *Task::loadPrerequisiteTasks()
{
    KeyValues kv(*this, def_tab_tasks_prereq);
    kv.select().whereString(def_col_tprq_taskname, context().task);

    list<string> tasknames;
    while(kv.next()) {
        tasknames.push_back(kv.getString(def_col_tprq_taskprereq));
    }

    return new Task(*this, tasknames);
}

string Task::getOutputDataTable()
{
    return this->getString(def_col_task_outputs);
}

TaskProgress *Task::loadTaskProgress()
{
    return new TaskProgress(*this, context().task);
}

TaskProgress *Task::loadTaskProgress(const list<string>& seqnames)
{
    return new TaskProgress(*this, context().task, seqnames);
}

Interval *Task::loadOutputData()
{
    return new Interval(*this, this->getOutputDataTable());
}

Process* Task::loadProcesses(int id)
{
    return (new Process(*this, id));
}

string Task::getName()
{
    return this->getString(def_col_task_name);
}

TaskParams *Task::getParams()
{
    return new TaskParams(this->getString(def_col_task_params));
}

Process* Task::createProcess(const list<string>& seqnames)
{
    Process *p = NULL;
    int prsid = 0;
    bool retval = true;

    QueryBeginTransaction(*this).execute();

    {
        Insert insert(*this, def_tab_processes);
        retval &= insert.keyString(def_col_prs_taskname, context().task);

        if (retval && (retval = insert.execute())) {
            QueryLastInsertedId q(*this);
            if (retval = q.execute()) {
                prsid = q.getLastId();
                for (auto & item : seqnames) {
                    Insert insert2(*this, def_tab_processes_seq);
                    retval &= insert2.keyInt(def_col_prss_prsid, prsid);
                    retval &= insert2.keyString(def_col_prss_seqname, item);
                    if (retval && (retval = insert2.execute())) {
                    }
                    else {
                        break;
                    }
                }
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

IntervalOutput *Task::createIntervalOutput(const string &seqname)
{
    return new IntervalOutput(*this, seqname, this->getOutputDataTable());
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
    return update().whereString(def_col_task_name, context().task);
}

}
