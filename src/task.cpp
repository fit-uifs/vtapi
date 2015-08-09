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
#include <vtapi/common/defs.h>
#include <vtapi/queries/insert.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/task.h>


using namespace std;

namespace vtapi {


Task::Task(const Commons& commons, const string& name)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        throw exception();
    
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

Method *Task::getParentMethod()
{
    string mtname;
    if (!context().method.empty())
        mtname = context().method;
    else
        mtname = this->getString(def_col_task_mtname);

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

Sequence *Task::getSequenceLock(const string &seqname)
{
    if (!seqname.empty())
        return NULL;

    KeyValues kv(*this, def_tab_tasks_seq);
    kv._select.whereString(def_col_tsd_taskname, context().task);
    kv._select.whereString(def_col_tsd_seqname, seqname);
    if (kv.next()) {
        return NULL;
    }
    else {
        Insert i(*this, def_tab_tasks_seq);
        i.keyString(def_col_tsd_taskname, context().task);
        i.keyString(def_col_tsd_seqname, seqname);
        i.keyBool(def_col_tsd_isdone, false);
        if (i.execute()) {
            Sequence *s = new Sequence(*this, seqname);
            if (s->next()) {
                return s;
            }
            else {
                delete s;
                return NULL;
            }
        }
        else
            return NULL;
    }
}

Task *Task::loadPrerequisiteTasks()
{
    KeyValues kv(*this, def_tab_tasks_prereq);
    kv._select.whereString(def_col_tprq_taskname, context().task);

    list<string> tasknames;
    while(kv.next()) {
        tasknames.push_back(kv.getString(def_col_tprq_taskprereq));
    }

    return new Task(*this, tasknames);
}

Interval *Task::loadOutputData()
{
    return new Interval(*this, this->getString(def_col_task_outputs));
}

Sequence *Task::loadSequencesInProgress()
{
    KeyValues kv(*this, def_tab_tasks_seq);
    kv._select.whereString(def_col_tsd_taskname, context().task);
    kv._select.whereBool(def_col_tsd_isdone, false);

    list<string> seqnames;
    while (kv.next()) {
        seqnames.push_back(kv.getString(def_col_tsd_seqname));
    }

    return new Sequence(*this, seqnames);
}

Sequence *Task::loadSequencesFinished()
{
    KeyValues kv(*this, def_tab_tasks_seq);
    kv._select.whereString(def_col_tsd_taskname, context().task);
    kv._select.whereBool(def_col_tsd_isdone, true);

    list<string> seqnames;
    while (kv.next()) {
        seqnames.push_back(kv.getString(def_col_tsd_seqname));
    }

    return new Sequence(*this, seqnames);
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

bool Task::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_tasks);
    if (ret) {
        ret &= _update->whereString(def_col_task_name, context().task);
    }

    return ret;
}

}
