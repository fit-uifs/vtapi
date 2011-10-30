/* 
 * File:   VTApi.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:42
 */

#include <cstdlib>
#include <iostream>

#include "cli_settings.h"
#include "vtapi.h"


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
    while (1) {
        // get command
        if (line.empty()) getline(cin, line);
        if (cin.fail()) break;
        command = getWord(line);

        // exit
        if (command.compare("exit") == 0) break;
        // general query
        else if (command.compare("query") == 0) {
            PGresult *res = PQexecf(commons->getConnector()->getConn(),
                line.c_str());
            commons->printRes(res);
            PQclear(res);
        }
        // TODO: select
        else if (command.compare("select") == 0) {
            String what;
            what = getWord(line);
            cout << "todo" << endl;
        }
        // insert
        else if (command.compare("insert") == 0) {

            String input;
            input = getWord(line);

            CLIInsert* insert = new CLIInsert(new Dataset(*this->commons));
            while (!line.empty()) insert->addParam(getWord(line));

            if (!input.compare("dataset")) insert->setType(CLIInsert::DATASET);
            else if (!input.compare("sequence")) insert->setType(CLIInsert::SEQUENCE);
            else if (!input.compare("interval")) insert->setType(CLIInsert::INTERVAL);
            else if (!input.compare("process")) insert->setType(CLIInsert::PROCESS);
            else if (!input.compare("method")) insert->setType(CLIInsert::METHOD);

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
    cout << "TESTING generic classes:" << endl;
    TKeyValue<float> tkvf ("float", "number", 32156.7, "test");
    cout << "TKeyValue<float> tkvf (32156.7) ... typeid: " << typeid(tkvf).name() << endl;
    tkvf.print();

    cout << "std::vector<TKey*> ..." << endl;
    std::vector<TKey*> v;
    v.push_back(&tkvf);
    TKey* kv2 = new  TKeyValue<String> ("varchar", "string", "whatever", "test");
    v.push_back(kv2);
    for (int i = 0; i < v.size(); ++i) (v[i])->print();
    cout << "static_cast< TKeyValue<float>* >(v[0]);" << endl;
    TKeyValue<float>* tkfc = static_cast< TKeyValue<float>* >(v[0]);
    tkfc->print();
    cout << endl;

    cout << "TESTING  Dataset" << endl;
    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printRes(dataset->select->res);

    cout << endl;
    cout << "TESTING Sequence" << endl;
    cout << "USING dataset " << dataset->getDataset() << endl;

    Sequence* sequence = dataset->newSequence();
    sequence->next();
    sequence->printRes(sequence->select->res);

    cout << endl;
    cout << "TESTING Interval" << endl;
    cout << "USING sequence " << sequence->getSequence() << endl;
    
    Interval* interval = sequence->newInterval();
    interval->select->limit = 10;
    interval->next();
    cout << "// FIXME: PQprint is deprecated" << endl;
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

