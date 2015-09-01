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
    if (_context.dataset.empty())
        throw BadConfigurationException("dataset not specified");
    
    if (id != 0)
        _context.process = id;
    
    _select.setOrderBy(def_col_prs_prsid);
    
    if (_context.process != 0) {
        _select.querybuilder().whereInt(def_col_prs_prsid, _context.process);
    }
    else {
        if (!_context.task.empty())
            _select.querybuilder().whereString(def_col_prs_taskname, _context.task);
    }
}

Process::Process(const Commons& commons, const vector<int>& ids)
    : KeyValues(commons, def_tab_processes)
{
    if (_context.dataset.empty())
        throw BadConfigurationException("dataset not specified");
    
    _select.setOrderBy(def_col_prs_prsid);
    _select.querybuilder().whereIntVector(def_col_prs_prsid, ids);
    
    _context.task.clear();
}

bool Process::next()
{
    bool ret = KeyValues::next();
    if (ret) {
        _context.process = this->getId();
        return true;
    }
    else {
        return false;
    }
    return ret;
}

InterProcessServer * Process::initializeInstance(InterProcessServer::IModuleControlInterface & control)
{
    // delete temporary config file created for process
    string tmpdir = Poco::Path::temp();
    if (config().configfile.find(tmpdir) == 0)
        Poco::File(config().configfile).remove();

    return new InterProcessServer(constructUniqueName(), control);
}

InterProcessClient * Process::launchInstance()
{
    // create temporary config file
    string config_path = Poco::Path::temp();
    config_path += "vtproc_" + toString<int>(_context.process) + ".conf";

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
                    toString<int>(_context.process) + ";" + config_path);

        Poco::ProcessHandle hproc = Poco::Process::launch("vtmodule", args);
        VTLOG_DEBUG("Launched PID " + toString<Poco::ProcessHandle::PID>(hproc.id()));

        return new InterProcessClient(constructUniqueName(), this->getInstancePID(), hproc);
    }
    catch(exception& e) {
        VTLOG_ERROR("Failed to launch process : " +
                    toString<int>(_context.process) + ";" + e.what());
        Poco::File(config_path).remove();
    }

    return NULL;
}


InterProcessClient *Process::connectToInstance()  const
{
    return new InterProcessClient(constructUniqueName(), this->getInstancePID());
}

bool Process::isInstanceRunning() const
{
    return InterProcessClient(constructUniqueName(), this->getInstancePID()).isRunning();
}

Dataset *Process::getParentDataset() const
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

string Process::getParentTaskName() const
{
    if (!_context.task.empty())
        return _context.task;
    else
        return this->getString(def_col_prs_taskname);
}

Task *Process::getParentTask() const
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

string vtapi::Process::getParentMethodName() const
{
    string mtname;
    Task *ts = getParentTask();
    if (ts) {
        mtname = ts->getParentMethodName();
        delete ts;
    }

    return mtname;
}

Method *Process::getParentMethod() const
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

Video *Process::loadAssignedVideos() const
{
    vector<string> seqnames;
    KeyValues kv(*this, def_tab_processes_seq);
    kv._select.querybuilder().whereInt(def_col_prss_prsid, _context.process);
    while(kv.next()) {
        seqnames.push_back(kv.getString(def_col_prss_seqname));
    }

    return new Video(*this, seqnames);
}

ImageFolder *Process::loadAssignedImageFolders() const
{
    vector<string> seqnames;
    KeyValues kv(*this, def_tab_processes_seq);
    kv._select.querybuilder().whereInt(def_col_prss_prsid, _context.process);
    while(kv.next()) {
        seqnames.push_back(kv.getString(def_col_prss_seqname));
    }

    return new ImageFolder(*this, seqnames);
}

TaskProgress *Process::acquireSequenceLock(const string &seqname) const
{
    if (!seqname.empty() && !TaskProgress(*this, this->getParentTaskName(), seqname).next()) {
        Insert i(*this, def_tab_tasks_seq);
        i.querybuilder().keyString(def_col_tsd_taskname, this->getParentTaskName());
        i.querybuilder().keyString(def_col_tsd_seqname, seqname);
        i.querybuilder().keyInt(def_col_tsd_prsid, _context.process);
        if (i.execute()) {
            TaskProgress *prog = new TaskProgress(*this, this->getParentTaskName(), seqname, true);
            if (!prog->next())
                vt_destruct(prog);
            return prog;
        }
    }

    return NULL;
}

//////////////////////////////////////////////////
// getters - SELECT
//////////////////////////////////////////////////

int Process::getId() const
{
    return this->getInt(def_col_prs_prsid);
}

ProcessState Process::getState() const
{
    return this->getProcessState(def_col_prs_state);
}

int Process::getInstancePID() const
{
    return this->getInt(def_col_prs_pid);
}

std::string Process::getInstanceName() const
{
    return this->getString(def_col_prs_ipcname);
}

chrono::system_clock::time_point Process::getCreatedTime() const
{
    return this->getTimestamp(def_col_prs_created);
}

//////////////////////////////////////////////////
// updaters - UPDATE
//////////////////////////////////////////////////

bool Process::preUpdate()
{
    return update().querybuilder().whereInt(def_col_prs_prsid, _context.process);
}

bool Process::updateState(const ProcessState& state)
{
    bool ret = true;
    
    ret &= updateProcessStatus(def_col_prs_pstate_status, state.status);
    
    switch (state.status)
    {
    case ProcessState::STATUS_CREATED:
        ret &= updateFloat8(def_col_prs_pstate_progress, 0.0);
        ret &= updateString(def_col_prs_pstate_curritem, string());
        ret &= updateString(def_col_prs_pstate_errmsg, string());
        break;
    case ProcessState::STATUS_RUNNING:
        ret &= updateFloat8(def_col_prs_pstate_progress, state.progress);
        ret &= updateString(def_col_prs_pstate_curritem, state.current_item);
        break;
    case ProcessState::STATUS_FINISHED:
        break;
    case ProcessState::STATUS_ERROR:
        ret &= updateString(def_col_prs_pstate_errmsg, state.last_error);
        break;
    }

    return ret;
}

bool Process::updateInstancePID(int pid)
{
    return this->updateInt(def_col_prs_pid, pid);
}

bool Process::updateInstanceName(const string &name)
{
    return this->updateString(def_col_prs_ipcname, name);
}

//////////////////////////////////////////////////
// filters/utilities
//////////////////////////////////////////////////

void Process::filterByTask(const string &taskname)
{
    _select.querybuilder().whereString(def_col_prs_taskname, taskname);
}

string Process::constructUniqueName() const
{
    return _context.dataset + '_' + toString(this->getId());
}




}
