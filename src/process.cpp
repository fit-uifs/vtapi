/**
 * @file
 * @brief   Methods of Process class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <exception>
#include <sstream>
//#include <boost/filesystem.hpp>
#include <vtapi/common/global.h>
#include <vtapi/common/defs.h>
#include <vtapi/data/process.h>

using namespace std;

namespace vtapi {


Process::Process(const Commons& commons, int id)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        throw exception();
    
    if (id != 0)
        context().process = id;
    
    _select.from(def_tab_processes, def_col_all);
    
    if (context().process != 0) {
        _select.whereInt(def_col_prs_prsid, context().process);
    }
    else {
        if (!context().task.empty())
            _select.whereString(def_col_prs_taskname, context().task);
    }

}

Process::Process(const Commons& commons, const list<int>& ids)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        VTLOG_WARNING("Dataset is not specified");
    
    _select.from(def_tab_processes, def_col_all);

    _select.whereIntInList(def_col_prs_prsid, ids);
    
    context().task.clear();
}

Process::~Process()
{}

bool Process::next()
{
    _instance.close();
    
    bool ret = KeyValues::next();
    if (ret) {
        context().process = this->getId();
        return true;
    }
    else {
        return false;
    }
    return ret;
}

bool Process::run(bool async, bool suspended, ProcessControl **ctrl)
{
    bool ret = false;
    
//    do {
//        if (suspended) {
//            updateStateSuspended();
//        }
        
//        boost::filesystem::path cdir = boost::filesystem::current_path();
//        if (cdir.empty()) {
//            VTLOG_ERROR("Failed to get current directory");
//            break;
//        }

//        //TODO: dodelat
        
//        boost::filesystem::path exec(cdir);
//        exec /= "modules";
//        exec /= context().method;

//        boost::filesystem::path cfg = boost::filesystem::absolute(config().configfile, cdir);

//        compat::ProcessInstance::Args args;
//        args.push_back("--config=" + cfg.string());
//        args.push_back("--process=" + context().process);
//        args.push_back("--dataset=" + context().dataset);

//        ret = _instance.launch(exec.string(), args, !async);
//        if (ret) {
//            if (ctrl) *ctrl = new ProcessControl(this->getId(), _instance);
//        }
//        else {
//            VTLOG_ERROR("Failed to launch process " + toString(this->getId()) + ": " + exec.string());
//        }
        
//    } while(0);
    
    return ret;
}

Dataset *Process::getParentDataset()
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

Task *Process::getParentTask()
{
    string taskname;
    if (!context().task.empty())
        taskname = context().task;
    else
        taskname = this->getString(def_col_prs_taskname);

    if (!taskname.empty()) {
        Task *t = new Task(*this, taskname);
        if (t->next()) {
            return t;
        }
        else {
            delete t;
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

Method *Process::getParentMethod()
{
    Task *t = getParentTask();
    if (t) {
        Method *m = t->getParentMethod();
        delete t;
        return m;
    }
    else {
        return NULL;
    }
}

Sequence *Process::loadAssignedSequences()
{
    list<string> seqnames;
    KeyValues kv(*this, def_tab_processes_seq);
    kv._select.whereInt(def_col_prss_prsid, context().process);
    while(kv.next()) {
        seqnames.push_back(kv.getString(def_col_prss_seqname));
    }

    return new Sequence(*this, seqnames);
}



//////////////////////////////////////////////////
// getters - SELECT
//////////////////////////////////////////////////

int Process::getId()
{
    return this->getInt(def_col_prs_prsid);
}

ProcessState *Process::getState()
{
    return this->getProcessState(def_col_prs_state);
}

//////////////////////////////////////////////////
// updaters - UPDATE
//////////////////////////////////////////////////

bool Process::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_processes);
    if (ret) {
        ret &= _update->whereInt(def_col_prs_prsid, context().process);
    }

    return ret;
}

bool Process::updateState(const ProcessState& state, ProcessControl *control)
{
    bool retval = true;
    
    retval &= updateProcessStatus(def_col_prs_pstate_status, state.status);
    
    switch (state.status)
    {
    case ProcessState::STATUS_RUNNING:
        retval &= updateFloat(def_col_prs_pstate_progress, state.progress);
        retval &= updateString(def_col_prs_pstate_curritem, state.currentItem);
        break;
    case ProcessState::STATUS_FINISHED:
        break;
    case ProcessState::STATUS_ERROR:
        retval &= updateString(def_col_prs_pstate_errmsg, state.lastError);
        break;
    case ProcessState::STATUS_SUSPENDED:
        break;
    default:
        retval = false;
        break;
    }

    if (retval) {
        retval = updateExecute();
        if (retval && control) {
            retval = control->notify(state);
        }
    }
    else {
        preUpdate();
    }

    return retval;
}

bool Process::updateStateRunning(float progress, const string& currentItem, ProcessControl *control)
{
    return updateState(
        ProcessState(ProcessState::STATUS_RUNNING, progress, currentItem), control);
}

bool Process::updateStateSuspended(ProcessControl *control)
{
    return updateState(
        ProcessState(ProcessState::STATUS_SUSPENDED, 0, ""), control);
}

bool Process::updateStateFinished(ProcessControl *control)
{
    return updateState(
        ProcessState(ProcessState::STATUS_FINISHED, 0, ""), control);
}

bool Process::updateStateError(const string& lastError, ProcessControl *control)
{
    return updateState(
        ProcessState(ProcessState::STATUS_ERROR, 0, lastError), control);
}

//////////////////////////////////////////////////
// controls - commands to process instance
//////////////////////////////////////////////////

ProcessControl *Process::getProcessControl()
{
    if (_select.resultset().getPosition() >= 0) {
        return new ProcessControl(context().process, _instance);
    }
    else {
        return NULL;
    }
}

bool Process::controlResume(ProcessControl *control)
{
    return control ? control->control(ProcessControl::COMMAND_RESUME) : false;
}

bool Process::controlSuspend(ProcessControl *control)
{
    return control ? control->control(ProcessControl::COMMAND_SUSPEND) : false;
}

bool Process::controlStop(ProcessControl *control)
{
    return control ? control->control(ProcessControl::COMMAND_STOP) : false;
}

//////////////////////////////////////////////////
// filters/utilities
//////////////////////////////////////////////////



}
