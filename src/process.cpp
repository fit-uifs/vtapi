/**
 * @file    process.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Process class
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

    string query;

    //TODO: for SQLite
//    if (backend == POSTGRES) {
    query = "SELECT P.*, PA.relname AS outputs_str\n"
            "  FROM " + this->getDataset() + ".processes P\n"
            "  LEFT JOIN pg_catalog.pg_class PA ON P.outputs::regclass = PA.relfilenode";
//    }
//    else {
//        query = "\nSELECT * from public.processes";
//    }

    this->select = new Select(orig, query.c_str());

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
    return this->getString("outputs_str");
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

int Process::getParamInt(const std::string& key) {
    for (size_t i = 0; i < this->params.size(); i++) {
        if (this->params[i]->key.compare(key) == 0) {
            return ((TKeyValue<int> *)this->params[i])->values[0];
        }
    }
    return -1;
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
    this->params.push_back(new TKeyValue<int>("int", key, value));
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
    
    // this is the fun
    if (retval) {
        
        //retval = this->prepareOutput(method, selection);
//        update = new Update(*this, "ALTER TABLE \""+ selection +"\" ADD COLUMN \""+ name +"\" real[];");
//        retval &= update->execute();
    }
    
//    vt_destruct(update);

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

bool Process::prepareOutput(const string& method, const string& selection) {
    // podívej se, jestli ve výstupní tabulce "selection" je tento sloupec připraven
    // pokud ve výstupní tabulce sloupec není, vytvoř jej
    
    bool retval = true;

    Method* mth = new Method(*this, method);
    mth->next();
    TKeys methodKeys = mth->getMethodKeys();
    
    map<string,string> colsToAdd = this->diffColumns(selection, methodKeys);
    if (retval = colsToAdd.size()) {
      retval = this->addColumns(selection, colsToAdd);
    }
    
    vt_destruct(mth);
    
    return retval;
}

map<string,string> Process::diffColumns(const string& table, const TKeys& methodKeys) {
    map<string,string> tableCols;
    map<string,string>::iterator iTableCols;
    map<string,string> colsToAdd;
    
    KeyValues* kv = new KeyValues(*this);
    
//    kv->select = new Select(*this, "SELECT column_name, data_type FROM information_schema.columns WHERE table_schema = '" + this->dataset + "' AND table_name   = '" + table + "'");
    // TODO: At the moment Postgres ONLY):( 
    kv->select = new Select(*this, "SELECT a.attname AS column_name, a.atttypid::regtype::text AS data_type FROM pg_catalog.pg_attribute a JOIN pg_catalog.pg_class r ON r.oid = a.attrelid JOIN pg_catalog.pg_namespace n ON n.oid = r.relnamespace WHERE n.nspname = '" + this->dataset + "' AND r.relname = '" + table + "' AND attstattarget = -1;");
    while (kv->next()) {
        tableCols.insert(pair<string,string>(kv->getString("column_name"), kv->getString("data_type")));
    }
   
    for (TKeys::const_iterator it = methodKeys.begin(); it != methodKeys.end(); ++it) {
        if (it->from == "out" || it->from == "inout") {
            iTableCols = tableCols.find(it->key);
            if (iTableCols == tableCols.end()) {
                colsToAdd.insert(pair<string,string>(it->key, it->type));
            }
            else {
                if (iTableCols->second != it->type) {
                    this->logger->error(360, "VTApi panic: Column " + it->key + " exists, but there is not the same type (selection's attribute type: " + iTableCols->second + " X method's attribute type: " + it->type + ")!", this->thisClass + "diffColumns()");
                }
            }
        }
    }
    
    vt_destruct(kv->select);
    vt_destruct(kv);
    
    return colsToAdd;
}

bool Process::addColumns(const string& table, const map<string,string>& colsToAdd) {
    bool retval = true;
    
    if (colsToAdd.size()) {
        string queryString = "ALTER TABLE " + table + " ";
        for (map<string,string>::const_iterator it = colsToAdd.begin(); it != colsToAdd.end(); it++) {
            queryString += "ADD COLUMN \"" + it->first + "\" " + it->second + ", ";
        }
        queryString.resize(queryString.size() - 2);
        
        Query* query = new Query(*this, queryString + ";");
        retval = query->execute();
        
        vt_destruct(query);
    }
    return retval;
}

bool Process::run() {
    
    if (this->select->resultSet->getPosition() == -1) {
        if (!next()) return false;
    }

    bool ret = true;

    if (this->callback) this->callback(STATE_STARTED, this, pCallbackContext);
    
    std::stringstream ss;
    ss << "./modules/" << this->method << " --process=" << this->process;
    
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
        if (params[i]->type.compare("int") == 0) {
            ss << ((TKeyValue<int> *)params[i])->values[0];
        }
        else if (params[i]->type.compare("double") == 0) {
            ss << ((TKeyValue<double> *)params[i])->values[0];
        }
        else if (params[i]->type.compare("string") == 0) {
            ss << ((TKeyValue<std::string> *)params[i])->getValue();
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
    int valInt;
    double valDouble;
    TKey *key = NULL;
    
    strcpy(str, paramString.c_str());
    
    token = strtok(str+1, ",");
    while(token) {
        value = strchr(token, ':');
        *(value++) = '\0';
        
        size_t len = strlen(value);
        if (value[len-1] == '}') value[len-1] = '\0';
        
        valInt = strtol(value, &endptr, 10);
        if (endptr && !*endptr) {
            key = new TKeyValue<int>("int", token, valInt);
        }
        else {
            valDouble = strtod(value, &endptr);
            if (endptr && !*endptr) {
                key = new TKeyValue<double>("double", token, valDouble);
            }
            else {
                key = new TKeyValue<std::string>("string", token, value);
            }
        }
        this->params.push_back(key);
        
        token = strtok(NULL, ",");
    }
    
    delete[] str;
}