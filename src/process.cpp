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
#include <Poco/AutoPtr.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Process.h>
#include <Poco/Util/PropertyFileConfiguration.h>
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
    _select.orderBy(def_col_prs_prsid);
    
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

bool Process::instantiateSelf()
{
    // delete temporary config file created for process
    string tmpdir = Poco::Path::temp();
    if (config().configfile.find(tmpdir) == 0)
        Poco::File(config().configfile).remove();

    return context().process != 0 && !_select.isExecuted() && next();
}

bool Process::launch(bool suspended)
{
    bool ret = false;

    if (suspended)
        updateStateSuspended();

    // create temporary config file
    string config_path = Poco::Path::temp();
    config_path += "vtproc_" + toString<int>(context().process) + ".conf";

    // save configuration
    Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> config =
            new Poco::Util::PropertyFileConfiguration();
    this->saveConfig(*config.get());
    config->save(config_path);

    // set config file through command line
    Poco::Process::Args args;
    args.push_back("--config=" + config_path + "");

    // try launching vtmodule
    try {
        VTLOG_DEBUG("Launching process... " +
                    toString<int>(context().process) + ";" + config_path);

        Poco::ProcessHandle hproc = Poco::Process::launch("vtmodule", args);
        VTLOG_DEBUG("Launched PID " + toString<Poco::ProcessHandle::PID>(hproc.id()));

        ret = true;
    }
    catch(exception& e) {
        VTLOG_ERROR("Failed to launch process : " +
                    toString<int>(context().process) + ";" + e.what());
        Poco::File(config_path).remove();
        updateStateError(e.what());
    }

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

vtapi::Video *vtapi::Process::loadAssignedVideos()
{
    list<string> seqnames;
    KeyValues kv(*this, def_tab_processes_seq);
    kv._select.whereInt(def_col_prss_prsid, context().process);
    while(kv.next()) {
        seqnames.push_back(kv.getString(def_col_prss_seqname));
    }

    return new Video(*this, seqnames);
}

vtapi::ImageFolder *vtapi::Process::loadAssignedImageFolders()
{
    list<string> seqnames;
    KeyValues kv(*this, def_tab_processes_seq);
    kv._select.whereInt(def_col_prss_prsid, context().process);
    while(kv.next()) {
        seqnames.push_back(kv.getString(def_col_prss_seqname));
    }

    return new ImageFolder(*this, seqnames);
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

int Process::getIpcPort()
{
    return this->getInt(def_col_prs_ipcport);
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

bool Process::updateState(const ProcessState& state)
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
    }
    else {
        preUpdate();
    }

    return retval;
}

bool Process::updateStateRunning(float progress, const string& currentItem)
{
    return updateState(ProcessState(ProcessState::STATUS_RUNNING, progress, currentItem));
}

bool Process::updateStateSuspended()
{
    return updateState(ProcessState(ProcessState::STATUS_SUSPENDED, 0, ""));
}

bool Process::updateStateFinished()
{
    return updateState(ProcessState(ProcessState::STATUS_FINISHED, 0, ""));
}

bool Process::updateStateError(const string& lastError)
{
    return updateState(ProcessState(ProcessState::STATUS_ERROR, 0, lastError));
}

bool Process::updateIpcPort(int port)
{
    bool ret = this->updateInt(def_col_prs_ipcport, port);
    if (ret)
        ret = updateExecute();
    else
        preUpdate();

    return ret;
}

//////////////////////////////////////////////////
// filters/utilities
//////////////////////////////////////////////////

void Process::filterByTask(const string &taskname)
{
    _select.whereString(def_col_prs_taskname, taskname);
}




}
