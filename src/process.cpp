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

#include <unistd.h>
#include <sys/wait.h>
#include <common/vtapi_global.h>
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
    if (!this->process.empty()) {
        select->whereString("prsname", this->process);
    }
    if (!this->method.empty()) {
        this->select->whereString("mtname", this->method);
    }
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
    this->inputs.clear();

    return kv;
}

string Process::getName() {
    return this->getString("prsname");
}

ProcessState *Process::getState()
{
    return this->getProcessState("state");
}

bool Process::updateStateRunning(float progress, const string& currentItem, ProcessControl *control)
{
    bool retval = true;
    retval &= setPStatus("state,status", ProcessState::STATUS_RUNNING);
    retval &= setFloat("state,progress", progress);
    retval &= setString("state,current_item", currentItem);
    if (retval) {
        retval = setExecute();
        if (retval && control) {
            retval = control->notify(ProcessState(ProcessState::STATUS_RUNNING, progress, currentItem));
        }
    }

    return retval;
}

bool Process::updateStateSuspended(ProcessControl *control)
{
    bool retval = true;
    retval &= setPStatus("state,status", ProcessState::STATUS_SUSPENDED);
    if (retval) {
        retval = setExecute();
        if (retval && control) {
            retval = control->notify(ProcessState(ProcessState::STATUS_SUSPENDED));
        }
    }

    return retval;
}

bool Process::updateStateFinished(float progress, ProcessControl *control)
{
    bool retval = true;
    retval &= setPStatus("state,status", ProcessState::STATUS_FINISHED);
    retval &= setFloat("state,progress", progress);
    if (retval) {
        retval = setExecute();
        if (retval && control) {
            retval = control->notify(ProcessState(ProcessState::STATUS_FINISHED, progress));
        }
    }

    return retval;
}

bool Process::updateStateError(const string& lastError, ProcessControl *control)
{
    bool retval = true;
    retval &= setPStatus("state,status", ProcessState::STATUS_ERROR);
    retval &= setString("state,last_error", lastError);
    if (retval) {
        retval = setExecute();
        if (retval && control) {
            retval = control->notify(ProcessState(ProcessState::STATUS_ERROR, 0, lastError));
        }
    }
    
    return retval;
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
    bool retval = VT_OK;
    
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

    return VT_OK;
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

    char cdir[1024];
    if (getcwd(cdir, sizeof (cdir)) == NULL) {
        logger->error("getcwd failed :" + this->getName(), thisClass + "::run()");
        return false;
    }
    
    string arg0(cdir);
    arg0 += "/modules/";
    arg0 += this->method;
    string arg1("--config=");
    arg1 += this->configfile;
    string arg2("--process=");
    arg2 += this->process;

    const char *argv[4];
    argv[0] = arg0.c_str();
    argv[1] = arg1.c_str();
    argv[2] = arg2.c_str();
    argv[3] = NULL;
    
    pid_t cpid = fork();
    if (cpid == 0) {
        if (execv(arg0.c_str(), (char**)argv) < 0) {
            logger->error("execv failed :" + this->getName(), thisClass + "::run()");
        }
        exit(1);
    }
    else if (cpid > 0) {
        if (!async) {
            int ret = 0;
            if (waitpid(cpid, &ret, 0) == -1) {
                logger->error("waitpid failed :" + this->getName(), thisClass + "::run()");
                return false;
            }
            else if(WIFEXITED(ret) && WEXITSTATUS(ret) != 0) {
                logger->warning("child process exited with non-zero status", thisClass + "::run()");
                return false;
            }
            else {
                return true;
            }
        }
    }
    else {
        logger->error("fork failed :" + this->getName(), thisClass + "::run()");
        return false;
    }
    
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

std::string Process::constructName()
{
    std::stringstream ss;
    
    ss << this->method << "p";
    for (size_t i = 0; i < this->params.size(); i++) {
        if (params[i]->type.compare("int") == 0) {
            ss << "_" << ((TKeyValue<int> *)params[i])->values[0];
        }
        else if (params[i]->type.compare("double") == 0) {
            ss << "_" << ((TKeyValue<double> *)params[i])->values[0];
        }
        else if (params[i]->type.compare("string") == 0) {
            ss << "_" << ((TKeyValue<std::string> *)params[i])->getValue();
        }
    }
    
    if (!this->inputs.empty()) ss << "_" << this->inputs;
    
    return ss.str();
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
        size_t keyPos = (*paramString.begin() == '{' ? 1 : 0);
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
                if (*paramString.end() == '}') valLen--;
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
