/* 
 * File:   VTCli.cpp
 * Author: vojca
 * 
 * Created on November 3, 2011, 7:12 PM
 */

#include <cstdlib>
#include <iostream>

#include <time.h>
#include "vtapi.h"
#include <sstream>

#include "vtcli.h"
#include "vtapi_settings.h"


using namespace std;


VTCli::VTCli(int argc, char** argv){
    this->processArgs(argc, argv);
    this->vtapi = new VTApi(argc, argv);
}
VTCli::VTCli(const VTApi& api) {
    this->vtapi = new VTApi(api);
}

VTCli::~VTCli() {
    destruct(vtapi);
}

int VTCli::run() {
    this->interact = this->cmdline.empty();
    if (this->interact) {
        cout << "VTApi is running" << endl;
        cout << "Commands: query, select, insert, update, delete, show, test, help, exit" << endl;
    }
    String line, command;
    // command cycle
    do {
        // get command, if cli is empty then start interactive mode
        if (interact) getline(cin, line);
        else line = this->cmdline;
        // EOF detected
        if (cin.fail()) break;

        command = getWord(line);
        // command: exit
        if (!command.compare("exit")) break;
        // command: general query
        else if (!command.compare("query")) {
            if (!line.substr(0,4).compare("help")) {
                this->printHelp(command);
                continue;
            }
            if (line.empty()) continue;
            Query* query = new Query(*(this->vtapi->commons), line, NULL);
            query->execute();
            KeyValues *kv = new KeyValues(*(this->vtapi->commons));
            kv->printRes(query->res);
            destruct(kv);
            destruct(query);
        }
        // command: select
        else if (!command.compare("select")) {
            String input = getWord(line);
            // select help
            if (!input.compare("help")) {
                this->printHelp(command);
                continue;
            }
            // get select params
            std::map<String,String> params;
            while (!line.empty()) params.insert(createParam(getWord(line)));

            // TODO: udelat pres Tkey
            // select dataset
            if (!input.compare("dataset")) {
                Dataset* ds = new Dataset(*(this->vtapi->commons));
                ds->select->where.clear();
                ds->select->whereString("dsname", params["name"]);
                ds->select->whereString("dslocation", params["location"]);
                ds->next();
                ds->printAll();
                destruct(ds);
            }
            // select sequence
            else if (!input.compare("sequence")) {
                Sequence* seq = new Sequence(*(this->vtapi->commons));
                seq->select->where.clear();
                seq->select->whereString("seqname", params["name"]);
                //seq->select->whereInt("seqnum", atoi(params["seqnum"]));
                seq->select->whereString("seqlocation", params["location"]);
                seq->select->whereString("seqtyp", params["seqtype"]);
                seq->next();
                seq->printAll();
                destruct(seq);
            }
            // select interval
            else if (!input.compare("interval")) {
                Interval* in = new Interval(*(this->vtapi->commons));
                in->select->where.clear();
                in->select->whereString("seqname", params["sequence"]);
                //in->select->whereInt(t1, params["t1"]);
                //in->select->whereInt(t1, params["t1"]);
                in->select->whereString("imglocation", params["location"]);
                in->next();
                in->printAll();
                destruct(in);
            }
            // select process
            else if (!input.compare("process")) {
                Process* pr = new Process(*(this->vtapi->commons));
                pr->select->where.clear();
                pr->select->whereString("prsname", params["name"]);
                pr->select->whereString("mtname", params["method"]);
                pr->select->whereString("inputs", params["inputs"]);
                pr->select->whereString("outputs", params["outputs"]);
                pr->next();
                pr->printAll();
                destruct(pr);
            }
            // select method
            else if (!input.compare("method")) {
                Method* me = new Method(*(this->vtapi->commons));
                //TODO: methodkeys not implemented yet
                me->select->where.clear();
                me->select->whereString("mtname", params["name"]);
                me->next();
                me->printAll();
                destruct(me);
            }
        }
        // command: insert
        else if (command.compare("insert") == 0) {
            String input = getWord(line);
            if (!input.compare("help")) {
                this->printHelp(command);
                continue;
            }

            CLIInsert* insert = new CLIInsert(*(this->vtapi->commons));
            while (!line.empty()) insert->addParam(createParam(getWord(line)));

            if (!input.compare("dataset")) insert->setType(CLIInsert::DATASET);
            else if (!input.compare("sequence")) insert->setType(CLIInsert::SEQUENCE);
            else if (!input.compare("interval")) insert->setType(CLIInsert::INTERVAL);
            else if (!input.compare("process")) insert->setType(CLIInsert::PROCESS);
            else if (!input.compare("method")) insert->setType(CLIInsert::METHOD);
            else if (!input.compare("help")) {
                cout << "helping you with insert command " << endl;
            }
            insert->execute();
            destruct(insert);
        }
        // TODO: update
        else if (!command.compare("update")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                this->printHelp(command);
                continue;
            }
            this->vtapi->commons->warning("update command not implemented");
        }
        // TODO: delete
        else if (!command.compare("delete")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                this->printHelp(command);
                continue;
            }
            this->vtapi->commons->warning("delete command not implemented");
        }
        // TODO: show
        else if (!command.compare("show")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                this->printHelp(command);
                continue;
            }
            this->vtapi->commons->warning("show command not implemented");
        }
        // command: test
        else if (!command.compare("test")) {
            this->vtapi->test();
        }
        // command: help
        else if (!command.compare("help")) {
            this->printHelp();
        }
        else this->vtapi->commons->warning("Unknown command");

    } while(interact);

    return 0;
}
/**
 * Prints basic help
 * @return 0 on success, -1 on failure
 */
int VTCli::printHelp() {
    return this->printHelp("all");
}
/**
 * Prints help string
 * @param what Help context ("all", "select", ...)
 * @return 0 on success, -1 on failure
 */
int VTCli::printHelp(const String& what) {
    stringstream hss;

    if (this->helpStrings.count(what)) {
        cout << this->helpStrings[what];
        return 0;
    }
    else if (!what.compare("query")) {
        hss << endl <<
            "query [SQLSTRING]" << endl << endl <<
            "     * executes custom SQLQUERY" << endl <<
            "     * ex.: list methods with active processes" << endl <<
            "              query SELECT DISTINCT mtname FROM public.processes" << endl ;
    }
    else if (!what.compare("select")) {
        hss << endl <<
            "select dataset|sequence|interval|process|method|selection [ARGS]" << endl << endl <<
            "Selects data and prints them in specified format (-f option)" << endl << endl <<
            "ARG format:      arg=value or arg=value1,value2,..." << endl << endl <<
            " Dataset ARGS:" << endl <<
            "      name       name of the dataset" << endl <<
            "  location       base location of the dataset data files (directory)" << endl << endl <<
            " Sequence ARGS:" << endl <<
            "      name       name of the sequence" << endl <<
            "  location       location of the sequence data file(s) (file/directory)" << endl <<
            "    seqnum       unique number of the sequence" << endl <<
            "   seqtype       type of the sequence [images, video]" << endl << endl <<
            "Interval ARGS:" << endl <<
            "  sequence       name of the sequence containing this interval" << endl <<
            "        t1       begin time of the interval" << endl <<
            "        t2       end time of the interval" << endl <<
            "  location       location of the interval data file (file)" << endl << endl <<
            "Method ARGS:" << endl <<
            "      name       name of the method" << endl << endl <<
            "Process ARGS:" << endl <<
            "      name       name of the process" << endl <<
            "    method       name of the method the process is instance of" << endl <<
            "    inputs       data type of inputs (database table)" << endl <<
            "   outputs       data type of outputs (database table)" << endl << endl <<
            "Selection ARGS:" << endl <<
            "   (not implemented)" << endl;
    }
    else if (!what.compare("insert")) {
        hss << endl <<
            "insert sequence|interval|process [ARGS]" << endl << endl <<
            "Inserts data into database" << endl << endl <<
            "ARG format:      arg=value or arg=value1,value2,..." << endl << endl <<
            " Sequence ARGS:" << endl <<
            "      name       name of the sequence *REQUIRED*" << endl <<
            "  location       location of the sequence data file(s) (file/directory)" << endl <<
            "    seqnum       unique number of the sequence" << endl <<
            "   seqtype       type of the sequence [images, video]" << endl << endl <<
            "Interval ARGS:" << endl <<
            "  sequence       name of the sequence containing this interval *REQUIRED*" << endl <<
            "        t1       begin time of the interval *REQUIRED*" << endl <<
            "        t2       end time of the interval *REQUIRED*" << endl <<
            "  location       location of the interval data file (file)" << endl << endl <<
            "Process ARGS:" << endl <<
            "      name       name of the process *REQUIRED*" << endl <<
            "    method       name of the method the process is instance of" << endl <<
            "    inputs       data type of inputs (database table)" << endl <<
            "   outputs       data type of outputs (database table)" << endl;
    }
    else if (!what.compare("update")) {
        hss << "update command not implemented";
    }
    else if (!what.compare("delete")) {
        hss << "delete command not implemented";
    }
    else if (!what.compare("show")) {
        hss << "show command not implemented";
    }
    else hss.clear(_S_failbit);
    if (!hss.fail()) {
        this->helpStrings.insert(std::make_pair(what,hss.str()));
        cout << hss.str() << endl;
        return 0;
    }
    else return -1;
}

/**
 * Creates help and command strings from arguments
 * @param argc Argument count
 * @param argv Argument field
 * @return 0 on success, -1 on failure
 */
int VTCli::processArgs(int argc, char** argv) {
    gengetopt_args_info args_info;
    if (cmdline_parser2 (argc, argv, &args_info, 0, 1, 0) != 0) {
        helpStrings.insert(std::make_pair("all", ""));
        this->cmdline.clear();
        return -1;
    }
    else {
        // Construct help string
        stringstream hss;
        int i = 0;
        hss << endl << CMDLINE_PARSER_PACKAGE_NAME <<" "<< CMDLINE_PARSER_VERSION << endl;
        hss << endl << gengetopt_args_info_usage << endl << endl;
        while (gengetopt_args_info_help[i])
            hss << gengetopt_args_info_help[i++] << endl;
        helpStrings.insert(std::make_pair("all", hss.str()));
        // Construct command string from unnamed arguments
        for (int i = 0; i < args_info.inputs_num; i++)
            cmdline.append(args_info.inputs[i]).append(" ");
        cmdline_parser_free (&args_info);
        return 0;
    }  
}

/**
 * Creates key/value pair from string in key=value format
 * @param word Input string
 * @return Key,Value pair
 */
std::pair<String,String> VTCli::createParam(String word) {
    size_t pos = word.find('=');

    if (pos != string::npos && pos < word.length()-1)
        return pair<String,String>
                (word.substr(0,pos), word.substr(pos+1, string::npos));
    else
        return pair<String,String>("","");
}
/**
 * Cut first word from input command line
 * @param line input line
 * @return word
 */
String VTCli::getWord(String& line) {
    String word;
    size_t startPos = 0, pos, endPos = string::npos;

    if (line.empty()) return "";

    // word end is whitespace, skip quoted part
        pos = line.find_first_of(" \t\n\"", startPos);
        if (pos != string::npos && line.at(pos) == '\"'){
            if (pos + 1 >= line.length()) {
                line.clear();
                return "";
            }
            endPos = line.find('\"', pos + 1);
            if (endPos == string::npos)
                word = line.substr(pos + 1, string::npos);
            else {
                word = line.substr(pos + 1, endPos - pos - 1);
                endPos++;
            }
        }
        else {
            endPos = pos;
            word = line.substr(0, endPos);
        }
    // cut line
    if (endPos != string::npos && endPos < line.length()){
        endPos = line.find_first_not_of(" \t\n", endPos);
        if (endPos != string::npos) line = line.substr(endPos, string::npos);
        else line.clear();
    }
    else
        line.clear();

    return word;
}
/**
 * Cuts first comma-separated value from string
 * @param word CSV string
 * @return first CSV
 */
String VTCli::getCSV(String& word) {
    String csv;
    size_t pos = word.find(",", 0);

    csv = word.substr(0, pos);
    if (pos != string::npos && pos+1 < word.length())
        word = word.substr(pos+1, string::npos);
    else word.clear();
    return csv;
}





// ************************************************************************** //
CLIInsert::CLIInsert(Commons& orig) {
    this->connector = (&orig)->getConnector();
    this->dataset = (&orig)->getDataset();
    this->type = GENERIC;
}
CLIInsert::~CLIInsert() {
    this->params.clear();
}

void CLIInsert::setType(InsertType newtype){
    this->type = newtype;
}

int CLIInsert::addParam(pair<String,String> param) {
    pair<map<String,String>::iterator,bool> ret;

    if (!param.first.empty() && !param.second.empty()) {
	ret = this->params.insert(param);
        // key already existed
        if (!ret.second) return -1;
        else return 0;
    }
    else return -1;
}

int CLIInsert::addParam(String key, String value) {
    return this->addParam(pair<String, String> (key, value));
}

String CLIInsert::getParam(String pname) {
    return this->params[pname];
}

void CLIInsert::getIntArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = string::npos;
    int tag;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConn());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        tag = atoi(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%int4", tag);
        if (endPos == string::npos) break;
        startPos = endPos + 1;
    }
}

void CLIInsert::getFloatArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = string::npos;
    float svm;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConn());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        svm = (float) atof(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%float4", svm);
        if (endPos == string::npos) break;
        startPos = endPos + 1;
    }
}

bool CLIInsert::checkLocation(String seqname, String intlocation) {

    String location;
    PGresult* res;
    PGtext seqlocation = (PGtext) "";

    // get sequence location
    res = PQexec(this->connector->getConn(),
        String("SELECT seqlocation FROM " + this->dataset +
            ".sequences WHERE seqname=\'" + seqname.c_str() + "\';").c_str());
    if(!res) {
        this->connector->getLogger()->write(PQgeterror());
        return false;
    }
    PQgetf(res, 0, "%varchar", 0, &seqlocation);

    // FIXME: check location using getDataLocation();
    location = this->dataset.append((String) seqlocation).append(intlocation);
    // TODO:

    PQclear(res);
    return true;

}

bool CLIInsert::execute() {

    stringstream query;
    PGresult *res;
    PGarray arr;
    PGparam *param = PQparamCreate(this->connector->getConn());
    bool queryOK = true;
    PGtimestamp timestamp = getTimestamp();

    if (this->type == GENERIC) {
        this->connector->getLogger()->write("Error: insert command incomplete\n");
        return 0;
    }

    //TODO: datasets/methods/selections
    // insert sequence
    if (this->type == SEQUENCE) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%int4", atoi(getParam("seqnum").c_str()));
        PQputf(param, "%varchar", getParam("location").c_str());
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".sequences " <<
            "(seqname, seqnum, seqlocation, seqtyp, created) " <<
            "VALUES ($1, $2, $3, \'" << getParam("seqtype") << "\', $4)";
    }
    // insert interval
    else if (this->type == INTERVAL) {
        PQputf(param, "%name", getParam("sequence").c_str());
        PQputf(param, "%int4", atoi(getParam("t1").c_str()));
        PQputf(param, "%int4", atoi(getParam("t2").c_str()));
        PQputf(param, "%varchar", getParam("location").c_str());
        getIntArray(getParam("tags"), &arr);
        PQputf(param, "%int4[]", &arr);
        PQparamReset(arr.param);
        getFloatArray(getParam("svm"), &arr);
        PQputf(param, "%float4[]", &arr);
        PQparamClear(arr.param);
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".intervals " <<
            "(seqname, t1, t2, imglocation, tags, svm, created) " <<
            "VALUES ($1, $2, $3, $4, $5, $6, $7)";
        queryOK = checkLocation(getParam("sequence"), getParam("location"));
    }
    // insert process
    if (this->type == PROCESS) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%name", atoi(getParam("method").c_str()));
        PQputf(param, "%varchar", getParam("inputs").c_str());
        PQputf(param, "%varchar", getParam("outputs").c_str());
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".processes " <<
            "(prsname, mtname, inputs, outputs, created) VALUES ($1, $2, $3, $4, $5)";
    }

    if (queryOK) {
        res = PQparamExec(this->connector->getConn(), param, query.str().c_str(), 1);
        if(!res)
            this->connector->getLogger()->write(PQgeterror());
        else PQclear(res);
    }

    PQparamClear(param);

    return 1;
}

void CLIInsert::clear(){

    this->params.clear();
    this->type = GENERIC;
}

PGtimestamp CLIInsert::getTimestamp() {
    PGtimestamp timestamp;
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    timestamp.date.isbc  = 0;
    timestamp.date.year  = timeinfo->tm_year + 1900;
    timestamp.date.mon   = timeinfo->tm_mon;
    timestamp.date.mday  = timeinfo->tm_mday;
    timestamp.time.hour  = timeinfo->tm_hour;
    timestamp.time.min   = timeinfo->tm_min;
    timestamp.time.sec   = timeinfo->tm_sec;
    timestamp.time.usec  = 0;
    return timestamp;
}




























/**
 * The CLI main function
 * You can follow the
 *
 * @param argc
 * @param argv
 * @return sucess
 */
int main(int argc, char** argv) {

    VTCli* vtcli = new VTCli(argc, argv);

    vtcli->run();

    destruct (vtcli);

    return 0;
}










