/**
 * @file
 * @brief   Methods of Process class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <boost/filesystem.hpp>
#include <common/vtapi_global.h>
#include <data/vtapi_process.h>

using namespace std;

namespace vtapi {


Process::Process(const KeyValues& orig, const string& name) : KeyValues(orig)
{
    thisClass   = "Process";

    // like this because outputs is regtype which has to be converted on server
    this->select = new Select(orig);
    this->select->from("processes", "prsname");
    this->select->from("processes", "mtname");
    this->select->from("processes", "inputs");
    this->select->from("processes", "outputs::text");
    this->select->from("processes", "state");
    this->select->from("processes", "params");
    this->select->from("processes", "userid");
    this->select->from("processes", "created");
    this->select->from("processes", "notes");

    if (!name.empty()) {
        this->process = name;
    }
    
    if (!process.empty()) select->whereString("prsname", process);
    if (!method.empty()) select->whereString("mtname", method);
}

Process::~Process()
{}

bool Process::next()
{
    m_instance.close();
    
    bool ret = KeyValues::next();
    if (ret) {
        this->process   = this->getName();
        this->selection = this->getOutputTable();
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
    
    do {
        // run newly added process and set Process object to represent it
        if (this->insert) {
            if (!addExecute()) break;
            if (select->resultSet->isOk())
                select->resultSet->setPosition(-1);
            if (!next()) break;
        }

        if (suspended) {
            updateStateSuspended();
        }
        
        boost::filesystem::path cdir = boost::filesystem::current_path();
        if (cdir.empty()) {
            logger->error("failed to get current directory", thisClass + "::run()");
            break;
        }

        boost::filesystem::path exec(cdir);
        exec /= "modules";
        exec /= this->method;

        boost::filesystem::path cfg = boost::filesystem::absolute(this->configfile, cdir);

        compat::ProcessInstance::Args args;
        args.push_back("--config=" + cfg.string());
        args.push_back("--process=" + this->process);
        args.push_back("--dataset=" + this->dataset);

        ret = m_instance.launch(exec.string(), args, !async);
        if (ret) {
            if (ctrl) *ctrl = new ProcessControl(this->getName(), m_instance);
        }
        else {
            logger->warning("error launching process " + this->getName()
            + ": " + exec.string(), thisClass + "::run()");
        }
        
    } while(0);
    
    return ret;
}

string Process::constructName(const ProcessParams & params)
{
    string ret;

    ret += this->method;
    ret += 'p';

    string par = params.serializeAsName();
    if (!par.empty()) {
        ret += '_';
        ret += par;
    }

    return ret;
}

//////////////////////////////////////////////////
// getters - SELECT
//////////////////////////////////////////////////

string Process::getName() {
    return this->getString("prsname");
}

ProcessState *Process::getState()
{
    return this->getProcessState("state");
}

string Process::getInputProcessName()
{
    return this->getString("inputs");
}

string Process::getOutputTable()
{
    return this->getString("outputs");
}

Process *Process::getInputProcess()
{
    string inputs = getInputProcessName();

    return inputs.empty() ? NULL : new Process(*this, inputs);
}

Interval *Process::getInputData()
{
    string inputs = this->getInputProcessName();
    if (!inputs.empty()) {
        Process p(*this, inputs);
        p.next();
        return new Interval(*this, p.getOutputTable());
    }
    else {
        return NULL;
    }
}

Interval *Process::getOutputData()
{
    return new Interval(*this, this->getOutputTable());
}

ProcessParams *Process::getParams()
{
    ProcessParams *params = new ProcessParams(this->getString("params"));
    params->setInputProcessName(getInputProcessName());
    
    return params;
}

//////////////////////////////////////////////////
// adders - INSERT
//////////////////////////////////////////////////

bool Process::add(const std::string& outputs)
{
    bool retval = true;

    retval &= KeyValues::preAdd(this->getDataset() + ".processes");
    retval &= addString("mtname", this->method);
    retval &= addString("outputs", this->getDataset() + "." + (outputs.empty() ? this->method + "out" : outputs));

    return retval;
}

bool Process::addInputProcessName(const std::string& processName)
{
    m_inputProcess = processName;
    return true;
}

bool Process::addOutputTable(const std::string& table)
{
    return addString("outputs", this->getDataset() + "." + table);
}

bool Process::addParams(ProcessParams && params)
{
    m_params = std::move(params);
    return true;
}

bool Process::addExecute()
{
    if (this->process.empty()) {
        if (!m_inputProcess.empty()) {
            m_params.setInputProcessName(m_inputProcess);
        }
        else {
            m_params.getInputProcess(m_inputProcess);
        }
        this->process = constructName(m_params);
    }
    
    insert->keyString("prsname", this->process);
    insert->keyString("params", m_params.serialize());
    if (!m_inputProcess.empty()) insert->keyString("inputs", m_inputProcess);

    m_params.clear();
    m_inputProcess.clear();
    
    select->whereString("prsname", this->process);
    
    return KeyValues::addExecute();
}

//////////////////////////////////////////////////
// updaters - UPDATE
//////////////////////////////////////////////////

bool Process::preUpdate()
{
    bool ret = KeyValues::preUpdate("processes");
    if (ret) {
        ret &= update->whereString("prsname", this->process);
    }

    return ret;
}

bool Process::updateState(const ProcessState& state, ProcessControl *control)
{
    bool retval = true;
    
    retval &= updateProcessStatus("state,status", state.status);
    
    switch (state.status)
    {
    case ProcessState::STATUS_RUNNING:
        retval &= updateFloat("state,progress", state.progress);
        retval &= updateString("state,current_item", state.currentItem);
        break;
    case ProcessState::STATUS_FINISHED:
        break;
    case ProcessState::STATUS_ERROR:
        retval &= updateString("state,last_error", state.lastError);
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
    if (this->select->resultSet->isOk()) {
        return new ProcessControl(this->getName(), m_instance);
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

void Process::filterByInputProcessName(const std::string& processName)
{
    this->select->whereString("inputs", processName);
}

void Process::filterByOutputTable(const std::string& table)
{
    this->select->whereString("outputs", table);
}

bool Process::clearOutputData()
{
    Query q(*this,
        "DELETE FROM " + this->getOutputTable() +
        " WHERE prsname = '" + this->getName() + "';");
    return q.execute();
}

}
