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

#include <Poco/AutoPtr.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Process.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/delete.h>
#include <vtapi/data/process.h>

using namespace std;

namespace vtapi {


Process::Process(const Process &copy)
    : Process(dynamic_cast<const Commons&>(copy))
{
}

Process::Process(const Commons& commons, int id)
    : KeyValues(commons, def_tab_processes)
{
    if (context().dataset.empty())
        throw BadConfigurationException("dataset not specified");
    
    if (id != 0)
        context().process = id;
    
    select().setOrderBy(def_col_prs_prsid);
    
    if (context().process != 0) {
        select().whereInt(def_col_prs_prsid, context().process);
    }
    else {
        if (!context().task.empty())
            select().whereString(def_col_prs_taskname, context().task);
    }
}

Process::Process(const Commons& commons, const list<int>& ids)
    : KeyValues(commons, def_tab_processes)
{
    if (context().dataset.empty())
        throw BadConfigurationException("dataset not specified");
    
    select().setOrderBy(def_col_prs_prsid);
    select().whereIntInList(def_col_prs_prsid, ids);
    
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

InterProcessServer * Process::initializeInstance(InterProcessServer::fnCommandCallback callback)
{
    // delete temporary config file created for process
    string tmpdir = Poco::Path::temp();
    if (config().configfile.find(tmpdir) == 0)
        Poco::File(config().configfile).remove();

    return new InterProcessServer(*this, callback);
}

InterProcessClient * Process::launchInstance()
{
    // create temporary config file
    string config_path = Poco::Path::temp();
    config_path += "vtproc_" + toString<int>(context().process) + ".conf";

    // serialize configuration to temp directory
    Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> config(
            new Poco::Util::PropertyFileConfiguration());
    this->saveConfig(*config.get());
    config->save(config_path);

    // set temp config file through command line
    Poco::Process::Args args;
    args.push_back("--config=" + config_path + "");

    // try launching vtmodule
    try {
        VTLOG_DEBUG("Launching process... " +
                    toString<int>(context().process) + ";" + config_path);

        Poco::ProcessHandle hproc = Poco::Process::launch("vtmodule", args);
        VTLOG_DEBUG("Launched PID " + toString<Poco::ProcessHandle::PID>(hproc.id()));

        return new InterProcessClient(*this, hproc);
    }
    catch(exception& e) {
        VTLOG_ERROR("Failed to launch process : " +
                    toString<int>(context().process) + ";" + e.what());
        Poco::File(config_path).remove();
        updateStateError(e.what());
    }

    return NULL;
}


InterProcessClient *Process::connectToInstance()
{
    return new InterProcessClient(*this);
}

bool Process::isInstanceRunning()
{
    return InterProcessClient(*this).isRunning();
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

string Process::getParentTaskName()
{
    if (!context().task.empty())
        return context().task;
    else
        return this->getString(def_col_prs_taskname);
}

Task *Process::getParentTask()
{
    string taskname = getParentTaskName();
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

string vtapi::Process::getParentMethodName()
{
    string mtname;
    Task *ts = getParentTask();
    if (ts) {
        mtname = ts->getParentMethodName();
        delete ts;
    }

    return mtname;
}

Method *Process::getParentMethod()
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

Video *Process::loadAssignedVideos()
{
    list<string> seqnames;
    KeyValues kv(*this, def_tab_processes_seq);
    kv.select().whereInt(def_col_prss_prsid, context().process);
    while(kv.next()) {
        seqnames.push_back(kv.getString(def_col_prss_seqname));
    }

    return new Video(*this, seqnames);
}

ImageFolder *Process::loadAssignedImageFolders()
{
    list<string> seqnames;
    KeyValues kv(*this, def_tab_processes_seq);
    kv.select().whereInt(def_col_prss_prsid, context().process);
    while(kv.next()) {
        seqnames.push_back(kv.getString(def_col_prss_seqname));
    }

    return new ImageFolder(*this, seqnames);
}

TaskProgress *Process::lockAssignedSequence(const string &seqname)
{
    if (!seqname.empty() && !TaskProgress(*this, this->getParentTaskName(), seqname).next()) {
        Insert i(*this, def_tab_tasks_seq);
        i.keyString(def_col_tsd_taskname, this->getParentTaskName());
        i.keyString(def_col_tsd_seqname, seqname);
        i.keyInt(def_col_tsd_prsid, context().process);
        if (i.execute()) {
            TaskProgress *prog = new TaskProgress(*this, this->getParentTaskName(), seqname);
            if (!prog->next()) vt_destruct(prog);
            return prog;
        }
    }

    return NULL;
}

bool Process::unlockAssignedSequence(const string &seqname)
{
    Delete d(*this, def_tab_tasks_seq);
    return d.whereString(def_col_tsd_taskname, this->getParentTaskName()) &&
            d.whereString(def_col_tsd_seqname, seqname) &&
            d.execute();
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

int Process::getInstancePID()
{
    return this->getInt(def_col_prs_pid);
}

int Process::getInstancePort()
{
    return this->getInt(def_col_prs_port);
}

//////////////////////////////////////////////////
// updaters - UPDATE
//////////////////////////////////////////////////

bool Process::preUpdate()
{
    return update().whereInt(def_col_prs_prsid, context().process);
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
    return updateState(ProcessState(ProcessState::STATUS_SUSPENDED, 0, string()));
}

bool Process::updateStateFinished()
{
    return updateState(ProcessState(ProcessState::STATUS_FINISHED, 0, string()));
}

bool Process::updateStateError(const string& lastError)
{
    return updateState(ProcessState(ProcessState::STATUS_ERROR, 0, lastError));
}

bool Process::updateInstancePID(int pid)
{
    return this->updateInt(def_col_prs_pid, pid);
}

bool Process::updateInstancePort(int port)
{
    return this->updateInt(def_col_prs_port, port);
}

//////////////////////////////////////////////////
// filters/utilities
//////////////////////////////////////////////////

void Process::filterByTask(const string &taskname)
{
    select().whereString(def_col_prs_taskname, taskname);
}




}
