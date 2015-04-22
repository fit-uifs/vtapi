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
#include <data/vtapi_method.h>
#include <data/vtapi_process.h>

using std::string;
using std::map;
using std::pair;

using namespace vtapi;


Process::Process(const KeyValues& orig, const string& name) : KeyValues(orig) {
    thisClass   = "Process";
    callback    = NULL;
    pCallbackContext = NULL;

    // like this because outputs is regtype which has to be converted on server
    this->select = new Select(orig);
    this->select->from("processes", "prsname");
    this->select->from("processes", "mtname");
    this->select->from("processes", "inputs");
    this->select->from("processes", "outputs::text");
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

bool Process::next() {
    // process is being added
    if (insert) {
        if (this->process.empty()) this->process = constructName();
        insert->keyString("prsname", this->process);
        insert->keyString("params", serializeParams());
        insert->keyString("inputs", this->inputs);
//        insert->keyString("outputs", this->getDataset() + "." + outputs);
        select->whereString("prsname", this->process);
        this->inputs.clear();
    }
    
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->process   = this->getName();
        this->selection = this->getOutputs();
        deserializeParams(this->getString("params"));
    }

    return kv;
}

string Process::getName() {
    return this->getString("prsname");
}

Process::STATE_T Process::getState() {
    return (Process::STATE_T)this->getInt("state");
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

int Process::getParamInt(const std::string& key) {
    return (int)getParamDouble(key);
}

double Process::getParamDouble(const std::string& key) {
    for (size_t i = 0; i < this->params.size(); i++) {
        if (this->params[i]->key.compare(key) == 0) {
            return ((TKeyValue<double> *)this->params[i])->values[0];
        }
    }
    return -1;
}

std::string Process::getParamString(const std::string& key) {
    for (size_t i = 0; i < this->params.size(); i++) {
        if (this->params[i]->key.compare(key) == 0) {
            return ((TKeyValue<std::string> *)this->params[i])->getValue();
        }
    }
    return "";
}


void Process::setInputs(const std::string& processName) {
    this->inputs = processName;
}

void Process::setOutputs(const std::string& table) {
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

void Process::setCallback(fCallback callback, void *pContext) {
    this->callback = callback;
    this->pCallbackContext = pContext;
}

void Process::filterByInputs(const std::string& processName) {
    this->select->whereString("inputs", processName);
}

bool Process::add(const std::string& outputs) {
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

bool Process::run() {
    
    if (this->select->resultSet->getPosition() == -1) {
        if (!next()) return false;
    }

    bool ret = true;

    if (this->callback) this->callback(STATE_STARTED, this, pCallbackContext);
    
    std::stringstream ss;
    ss << "./modules/" << this->method << " --config=" << this->configfile << " --process=" << this->process;
    printf("%s\n", ss.str().c_str());
    ret = (std::system(ss.str().c_str()) == 0);
    
    if (this->callback) this->callback(ret ? STATE_DONE : STATE_ERROR, this, pCallbackContext);
    
    return ret;
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

void Process::deserializeParams(std::string paramString)
{
    for (size_t i = 0; i < this->params.size(); i++) delete this->params[i];
    this->params.clear();
    
    if (paramString.empty()) return;
    
    char *str = new char[paramString.length() + 1];
    char *token = NULL;
    char *value = NULL;
    char *endptr;
    double valDouble;
    
    
    strcpy(str, paramString.c_str());
    
    token = strtok(str+1, ",");
    while(token) {
        if (!(value = strchr(token, ':'))) break;
        *(value++) = '\0';
        
        size_t len = strlen(value);
        if (value[len-1] == '}') value[len-1] = '\0';
        
        TKey *key = NULL;
        if (*value == '\"') {
            char *end = strchr(++value, '\"');
            if (end) {
                *end = '\0';
                key = new TKeyValue<std::string>("string", token, value);
            }
        }
        else {
            valDouble = strtod(value, &endptr);
            if (endptr && !*endptr) {
                key = new TKeyValue<double>("double", token, valDouble);
            }
        }
        if (key) {
            this->params.push_back(key);
        }
        
        token = strtok(NULL, ",");
    }
    
    delete[] str;
}