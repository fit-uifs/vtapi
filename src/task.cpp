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

#include <vtapi/common/vtapi_global.h>
#include <vtapi/common/vtapi_defs.h>
#include <vtapi/queries/vtapi_insert.h>
#include <vtapi/queries/vtapi_predefined.h>
#include <vtapi/data/vtapi_task.h>


using namespace std;

namespace vtapi {


Task::Task(const Commons& commons, const string& name)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        VTLOG_WARNING("Dataset is not specified");
    
    if (!name.empty())
        context().task = name;
    
    // list columns, because of outputs::text
    _select.from(def_tab_tasks, def_col_task_name);
    _select.from(def_tab_tasks, def_col_task_mtname);
    _select.from(def_tab_tasks, def_col_task_params);
    _select.from(def_tab_tasks, def_col_task_outputs + "::text");
    _select.from(def_tab_tasks, def_col_task_created);
    
    if (!context().task.empty()) {
        _select.whereString(def_col_task_name, context().task);
    }
    else {
        if (!context().method.empty())
            _select.whereString(def_col_task_mtname, context().method);
    }
}

Task::Task(const Commons& commons, const list<string>& names)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        VTLOG_WARNING("Dataset is not specified");
    
    // list columns, because of outputs::text
    _select.from(def_tab_tasks, def_col_task_name);
    _select.from(def_tab_tasks, def_col_task_mtname);
    _select.from(def_tab_tasks, def_col_task_params);
    _select.from(def_tab_tasks, def_col_task_outputs + "::text");
    _select.from(def_tab_tasks, def_col_task_created);

    _select.whereStringInList(def_col_task_name, names);
    
    context().method.clear();
}

Task::~Task()
{
}

bool Task::next()
{
    if (KeyValues::next()) {
        context().task = this->getName();
        context().selection = this->getString(def_col_task_outputs);
        return true;
    }
    else {
        return false;
    }
}

string Task::getName()
{
    return this->getString(def_col_task_name);
}

Task *Task::getPrerequisiteTasks()
{
    KeyValues kv(*this, def_tab_tasks_prereq);
    kv._select.whereString(def_col_tprq_taskname, context().task);
    
    list<string> tasknames;
    while(kv.next()) {
        tasknames.push_back(kv.getString(def_col_tprq_taskprereq));
    }
    
    return new Task(*this, tasknames);
}

TaskParams *Task::getParams()
{
    return new TaskParams(this->getString(def_col_task_params));
}

Interval *Task::getOutputData()
{
    return new Interval(*this, this->getString(def_col_task_outputs));
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
            if (retval = QueryLastInsertedId(*this).execute(prsid)) {
                for (auto & item : seqnames) {
                    Insert insert2(*this, def_tab_processes_seq);
                    retval &= insert.keyInt(def_col_prss_prsid, prsid);
                    retval &= insert.keyString(def_col_prss_seqname, item);
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

Process* Task::loadProcesses(int id)
{
    return (new Process(*this, id));
}

bool Task::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_tasks);
    if (ret) {
        ret &= _update->whereString(def_col_task_name, context().task);
    }

    return ret;
}

}
