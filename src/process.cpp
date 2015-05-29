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

#include <common/vtapi_global.h>
#include <common/vtapi_compat.h>
#include <boost/filesystem.hpp>
#include <data/vtapi_method.h>
#include <data/vtapi_process.h>

using std::string;
using std::map;
using std::pair;

using namespace vtapi;


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

Process::~Process() {
    for (size_t i = 0; i < this->params.size(); i++) delete this->params[i];
}

bool Process::next()
{
    // process is being added
    if (insert) {
        if (this->process.empty()) this->process = constructName();
        insert->keyString("prsname", this->process);
        insert->keyString("params", serializeParams());
        insert->keyString("inputs", this->inputs);
//        insert->keyString("outputs", this->getDataset() + "." + outputs);
        select->whereString("prsname", this->process);
    }
    
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->process   = this->getName();
        this->selection = this->getOutputs();
    }
    
    destroyParams();
    inputs.clear();

    return kv;
}

string Process::getName() {
    return this->getString("prsname");
}

ProcessState *Process::getState()
{
    return this->getProcessState("state");
}

bool Process::updateState(const ProcessState& state, ProcessControl *control)
{
    bool retval = true;
    
    retval &= setPStatus("state,status", state.status);
    
    switch (state.status)
    {
    case ProcessState::STATUS_RUNNING:
        retval &= setFloat("state,progress", state.progress);
        retval &= setString("state,current_item", state.currentItem);
        break;
    case ProcessState::STATUS_FINISHED:
        retval &= setFloat("state,progress", state.progress);
        break;
    case ProcessState::STATUS_ERROR:
        retval &= setString("state,last_error", state.lastError);
        break;
    case ProcessState::STATUS_SUSPENDED:
        break;
    default:
        retval = false;
        break;
    }

    if (retval) {
        retval = setExecute();
        if (retval && control) {
            retval = control->notify(state);
        }
    }
    else {
        preSet();
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

bool Process::updateStateFinished(float progress, ProcessControl *control)
{
    return updateState(
        ProcessState(ProcessState::STATUS_FINISHED, progress, ""), control);
}

bool Process::updateStateError(const string& lastError, ProcessControl *control)
{
    return updateState(
        ProcessState(ProcessState::STATUS_ERROR, 0, lastError), control);
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

string Process::getInputs() {
    return this->getString("inputs");
}

string Process::getOutputs() {
    return this->getString("outputs");
}

Process *Process::getInputProcess() {
    string inputs = getInputs();
    
    return inputs.empty() ? NULL : new Process(*this, inputs);
}

Interval *Process::getInputData() {
    string inputs = this->getInputs();
    if (inputs.empty()) return NULL;
    
    Process p(*this, inputs);
    p.next();
    
    return p.getOutputData();
}

Interval *Process::getOutputData() {
    return new Interval(*this, this->getOutputs());
}

void Process::deleteOutputData() {
    Query q(*this, "DELETE FROM " + this->getOutputs() + " WHERE prsname = '" + this->getName() + "';");
    q.execute();
}

int Process::getParamInt(const std::string& key)
{
    if (this->params.empty()) {
        deserializeParams(this->getString("params"));
    }
    
    return (int)getParamDouble(key);
}

double Process::getParamDouble(const std::string& key)
{
    if (this->params.empty()) {
        deserializeParams(this->getString("params"));
    }
    
    for (size_t i = 0; i < this->params.size(); i++) {
        if (this->params[i]->key.compare(key) == 0) {
            return ((TKeyValue<double> *)this->params[i])->values[0];
        }
    }
    return -1;
}

std::string Process::getParamString(const std::string& key)
{
    if (this->params.empty()) {
        deserializeParams(this->getString("params"));
    }
    
    for (size_t i = 0; i < this->params.size(); i++) {
        if (this->params[i]->key.compare(key) == 0) {
            return ((TKeyValue<std::string> *)this->params[i])->getValue();
        }
    }
    return "";
}

void Process::setInputs(const std::string& processName)
{
    this->inputs = processName;
}

void Process::setOutputs(const std::string& table)
{
    if (insert) ;//insert->keyString("outputs", this->getDataset() + "." + table);
    else this->setString("outputs", this->getDataset() + "." + table);
}

void Process::setParamInt(const std::string& key, int value) {
    this->setParamDouble(key, (double)value);
}
void Process::setParamDouble(const std::string& key, double value) {
    this->params.push_back(new TKeyValue<double>("double", key, value));
}
void Process::setParamString(const std::string& key, const std::string& value) {
    this->params.push_back(new TKeyValue<std::string>("string", key, value));
}

void Process::filterByInputs(const std::string& processName) {
    this->select->whereString("inputs", processName);
}

bool Process::add(const std::string& outputs)
{
    bool retval = true;
    
    vt_destruct(insert);
    insert = new Insert(*this, "processes");
    insert->keyString("mtname", this->method);
    insert->keyString("outputs", this->getDataset() + "." + (outputs.empty() ? this->method + "out" : outputs));

    return retval;
}

bool Process::preSet() {
    vt_destruct(update);
    update = new Update(*this, "processes");
    update->whereString("prsname", this->process);

    return true;
}

Interval* Process::newInterval(const int t1, const int t2) {
    return new Interval(*this);
}

bool Process::run(bool async, bool suspended)
{
    if (!this->select->resultSet->isOk() && !next()) {
        return false;
    }
    
    if (suspended) {
        updateStateSuspended();
    }

    boost::filesystem::path cdir = boost::filesystem::current_path();
    if (cdir.empty()) {
        logger->error("failed to get current directory", thisClass + "::run()");
        return false;
    }
    
    boost::filesystem::path bin(cdir);
    bin /= "modules";
    bin /= this->method;
    
    boost::filesystem::path cfg = boost::filesystem::absolute(this->configfile, cdir);
    
    compat::ARGS_LIST args;
    args.push_back("--config=" + cfg.string());
    args.push_back("--process=" + this->process);
    args.push_back("--dataset=" + this->dataset);
    
    bool ret = compat::launchProcess(bin.string(), args, !async);
    if (!ret) {
        logger->warning("error launching process " + this->getName()
            + ": " + bin.string(), thisClass + "::run()");
    }
    
    return ret;
}

ProcessControl *Process::getProcessControl()
{
    if (this->select->resultSet->isOk()) {
        return new ProcessControl(this->getName());
    }
    else {
        return NULL;
    }
}

string Process::constructName()
{
    string ret;
    
    ret += method;
    ret += "p";
    
    for (size_t i = 0; i < this->params.size(); i++) {
        ret += "_";
        ret += params[i]->getValue();
    }
    
    if (!this->inputs.empty()) {
        ret += "_";
        ret += inputs;
    }
    
    return ret;
}

std::string Process::serializeParams()
{
    std::stringstream ss;

    ss << "{";
    
    for (size_t i = 0; i < this->params.size(); i++) {
        ss << params[i]->key << ":";
        if (params[i]->type.compare("double") == 0) {
            ss << ((TKeyValue<double> *)params[i])->values[0];
        }
        else if (params[i]->type.compare("string") == 0) {
            ss << "\"" << ((TKeyValue<std::string> *)params[i])->getValue() << "\"";
        }
        else {
            ss << "0";
        }
        if (i < this->params.size()-1) ss << ",";
    }
    
    ss << "}";

    return ss.str();
}

int Process::deserializeParams(const std::string& paramString)
{

    destroyParams();

    if (paramString.empty()) {
        return 0;
    }
    else {
        int ret = 0;
        size_t keyPos = (paramString[0] == '{' ? 1 : 0);
        size_t maxPos = paramString.length();

        do {
            bool isString = false;

            // find key end
            size_t valPos = paramString.find(':', keyPos);
            if (valPos == string::npos) break;

            // count key length
            size_t keyLen = valPos - keyPos;
            if (++valPos >= maxPos) {
                ret = -1;
                break;
            }

            // find value end, quoted => is string
            size_t valEndPos = string::npos;
            if (paramString[valPos] == '\"' && valPos < maxPos - 1) {
                valEndPos = paramString.find('\"', ++valPos);
                if (valEndPos == string::npos) {
                    ret = -1;
                    break;
                }
                isString = true;
            }
            else {
                valEndPos = paramString.find(',', valPos);
            }

            // count value length
            size_t valLen = 0;
            if (valEndPos == string::npos) {
                valLen = maxPos - valPos;
                if (paramString[maxPos-1] == '}') valLen--;
            }
            else {
                valLen = valEndPos - valPos;
            }

            // get substrings, construct key/value
            TKey *key = NULL;
            if (isString) {
                key = new TKeyValue<std::string>("string",
                    paramString.substr(keyPos, keyLen),
                    paramString.substr(valPos, valLen));
            }
            else {
                char *endptr = NULL;
                double vald = strtod(paramString.substr(valPos, valLen).c_str(), &endptr);
                if (endptr && !*endptr) {
                    key = new TKeyValue<double>("double",
                        paramString.substr(keyPos, keyLen),
                        vald);
                }
                else {
                    ret = -1;
                    break;
                }
            }
            if (key) {
                this->params.push_back(key);
            }

            // find next key
            if (valEndPos == string::npos) break;
            keyPos = paramString.find_first_not_of("\",", valEndPos);

        } while(keyPos != string::npos);
        
        return ret < 0 ? ret : this->params.size();
    }
}

void Process::destroyParams()
{
    if (!params.empty()) {
        for (size_t i = 0; i < this->params.size(); i++) {
            delete this->params[i];
        }
        this->params.clear();
    }
}
