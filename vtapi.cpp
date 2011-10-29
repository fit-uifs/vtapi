/* 
 * File:   VTApi.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:42
 */

#include <cstdlib>
#include <iostream>

#include "cli_settings.h"
#include "VTApi.h"


using namespace std;

VTApi::VTApi(int argc, char** argv) {

    gengetopt_args_info args_info;
    struct cmdline_parser_params *cli_params;

    // Initialize parser parameters structure
    cli_params = cmdline_parser_params_create();
    // Hold check for required arguments until config file is parsed
    cli_params->check_required = 0;

    // Parse cmdline first
    if (cmdline_parser_ext (argc, argv, &args_info, cli_params) != 0) {
        cmdline_parser_free (&args_info);
        free (cli_params);
        exit(1);
    }

    // Get the rest of arguments from config file, don't override cmdline
    cli_params->initialize = 0;
    cli_params->override = 0;
    cli_params->check_required = 1;

    // Parse config file
    if (cmdline_parser_config_file
        (args_info.config_arg, &args_info, cli_params) != 0) {
        cmdline_parser_free (&args_info);
        free (cli_params);
        exit(1);
    }
    // TODO: user authentization here
    
    // Create commons class to store connection etc.
    commons = new Commons(args_info);
    // Construct command entered through arguments
    for (int i = 0; i < args_info.inputs_num; i++)
        cmdline.append(args_info.inputs[i]).append(" ");
    cmdline_parser_free (&args_info);
    free (cli_params);
    // Interactive mode
    interact = true;
}

VTApi::~VTApi() {
}


Dataset* VTApi::newDataset(const String& name) {
    return (new Dataset(*commons, name));
};



int VTApi::run() {
/*  TODO: co je tohle za blbost? proc by se to melo dotazovat na vsechna data???
    Sequence* sequence = dataset->newSequence();
    Interval* interval = sequence->newInterval();
    Process* process = new Process(*dataset);
    Insert* insert = new Insert(dataset);
*/
    String line = cmdline;
    String command;
    cout << "commands: query, select, insert, update, delete, show, test, exit" << endl;

    // command cycle
    while (interact) {
        // get command
        if (line.empty()) getline(cin, line);
        else interact = false;
        // EOF detected
        if (cin.fail()) break;
        command = getWord(line);

        // exit
        if (command.compare("exit") == 0) interact = false;
        // general query
        else if (command.compare("query") == 0) {
            PGresult *res = PQexecf(commons->getConnector()->getConn(),
                line.c_str());
            commons->printRes(res);
            PQclear(res);
        }
        // select
        else if (command.compare("select") == 0) {
            Dataset* ds = this->newDataset();

            String input;
            // what to select
            input = getWord(line);
            std::map<String,String> params;
            // get select params
            while (!line.empty()) params.insert(createParam(getWord(line)));

            if (!input.compare("dataset")) {
                ds->select->where.clear();
                ds->select->whereString("dsname", params["name"]);
                ds->select->whereString("dslocation", params["location"]);
                ds->next();
                ds->print();
            }
            else if (!input.compare("sequence")) {
                Sequence* seq = ds->newSequence();
                seq->select->where.clear();
                seq->select->whereString("seqname", params["name"]);
                // TODO: whereInt
                //seq->select->whereInt("seqnum", atoi(params["seqnum"]));
                seq->select->whereString("seqlocation", params["location"]);
                seq->select->whereString("seqtyp", params["seqtype"]);
                seq->next();
                seq->print();
            }
            else if (!input.compare("interval")) {
                Interval* in = ds->newSequence()->newInterval();
                in->select->where.clear();
                in->select->whereString("seqname", params["sequence"]);
                // TODO: whereInt
                //in->select->whereInt(t1, params["t1"]);
                //in->select->whereInt(t1, params["t1"]);
                in->select->whereString("imglocation", params["location"]);
                in->next();
                in->print();
            }
            else if (!input.compare("process")) {
                Process* pr = new Process(*ds);
                pr->select->where.clear();
                pr->select->whereString("prsname", params["name"]);
                pr->select->whereString("mtname", params["method"]);
                pr->select->whereString("inputs", params["inputs"]);
                pr->select->whereString("outputs", params["outputs"]);
                pr->next();
                pr->print();
            }
            else if (!input.compare("method")) {
                Method* me = new Method(*ds);
                //TODO: methodkeys not implemented yet
                me->select->where.clear();
                me->select->whereString("mtname", params["name"]);
                me->next();
                me->print();
            }

        }
        // insert
        else if (command.compare("insert") == 0) {

            String input;
            input = getWord(line);

            Insert* insert = new Insert(new Dataset(*this->commons));
            while (!line.empty()) insert->addParam(createParam(getWord(line)));

            if (!input.compare("dataset")) insert->setType(Insert::DATASET);
            else if (!input.compare("sequence")) insert->setType(Insert::SEQUENCE);
            else if (!input.compare("interval")) insert->setType(Insert::INTERVAL);
            else if (!input.compare("process")) insert->setType(Insert::PROCESS);
            else if (!input.compare("method")) insert->setType(Insert::METHOD);

            insert->execute();
        }
        // TODO: commands
        else if (command.compare("update") == 0) cout << "todo" << endl;
        else if (command.compare("delete") == 0) cout << "todo" << endl;
        else if (command.compare("show") == 0) cout << "todo" << endl;
        // test
        else if (command.compare("test") == 0) {
            this->test();
        }
        else cout << "ERROR: unknown command: " << command << endl;

        line.clear();
    }

    return 0;
}
/**
 * Creates key/value pair from string in key=value format
 * @param word Input string
 * @return Key,Value pair
 */
std::pair<String,String> VTApi::createParam(String word) {
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
String VTApi::getWord(String& line) {
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


void VTApi::test() {
    cout << "Testing generic functions:" << endl;
    cout << "  32156(.7) toString: " << toString(32156) << " " <<  toString(32156.7) << endl;
    cout << endl;

    cout << "Testing  Dataset" << endl;
    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printRes(dataset->select->res);
    // FIXME: jaktoze to nebere dedicnost???

    cout << endl;
    cout << "Testing Sequence" << endl;
    cout << "Using dataset " << dataset->getDataset() << endl;

    Sequence* sequence = dataset->newSequence();
    sequence->next();
    sequence->printRes(sequence->select->res);

    cout << endl;
    cout << "Testing Interval" << endl;
    cout << "Using sequence " << sequence->getSequence() << endl;

    Interval* interval = sequence->newInterval();
    interval->select->limit = 10;
    interval->next();
    interval->print();
}

/*
void VTApi::testKeyValues() {
    cout << "========== BEGIN OF TESTING PART ==========" << endl;
    Dataset* dataset = newDataset();
    dataset->next();
    Sequence* sequence = dataset->newSequence();
    size_t velikost;
    Method* method = new Method(*dataset);
    method->next();
    method->getOutputData();

    cout << endl;

    Process* process = new Process(*dataset);
    process->printProcesses();

    cout << "=========== END OF TESTING PART ===========" << endl;

//    Method* method = new Method();
//    method->next();

    sequence->next();
    cout << "Sekvence [getInt: seqnum]: " << sequence->getInt("seqnum") << endl;
    cout << "Sekvence [getString: seqlocation]: " << sequence->getString("seqlocation") << endl;
    cout << "Sekvence [getString: seqname]: " << sequence->getString("seqname") << endl;
    Interval* interval = sequence->newInterval();
    std::vector<int> pole = interval->getIntV(4);
    cout << "Intervaly [getIntV: #4] - velikost pole " << velikost << ":" << endl;

    for (int i = 0; i < pole.size(); i++) {
        cout << i << ": " << pole[i] << endl;
    }
  
}*/

