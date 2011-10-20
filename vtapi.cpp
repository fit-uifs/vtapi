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

    // Create commons class to store connection etc.
    commons = new Commons(args_info);

    cmdline_parser_free (&args_info);
    free (cli_params);
}

VTApi::~VTApi() {
}


Dataset* VTApi::newDataset(const String& name) {
    if (name.empty()) return (new Dataset(*commons));
    // TODO: else
};



int VTApi::run() {


    Dataset* dataset = newDataset();
    Sequence* sequence = dataset->newSequence();
    Interval* interval = sequence->newInterval();
    Process* process = new Process(*dataset);
    Insert* insert = new Insert(commons->getConnector());

    dataset->next(); // first dataset (public)
    String line, command;

    cout << "commands: query, select, insert, update, delete, show, test, exit" << endl;

    // command cycle
    while (1) {
        // get command
        getline(cin, line);
        if (cin.fail()) break;
        command = getWord(line);

        // exit
        if (command.compare("exit") == 0) break;
        // general query (here?)
        else if (command.compare("query") == 0) {
            PGresult *res = PQexecf(commons->getConnector()->getConnection(),
                line.c_str());
            PQprintOpt opt = {0};
            opt.header    = 1;
            opt.align     = 1;
            opt.fieldSep  = (char *) "|";
            PQprint(stdout, res, &opt);
            if(!res)
                    cout << "ERROR: " << PQgeterror() << endl;
            PQclear(res);           
        }
        // select
        else if (command.compare("select") == 0) {
            String what;
            what = getWord(line);
            cout << "todo" << endl;
        }
        // insert
        else if (command.compare("insert") == 0) {

            String input, name;
            input = getWord(line);

            insert->clear();
            insert->setDataset(dataset->getName());
            while (!line.empty()) insert->addParam(getWord(line));

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

            Dataset* testDataset = newDataset();
            Test* test = new Test(*testDataset);
            test->testAll();
            delete test;
            delete testDataset;
        }
        else cout << "ERROR: unknown command: " << command << endl;
    }

    /* Deallocate memory */
    delete insert;
    delete process;
    delete interval;
    delete sequence;
    delete dataset;

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
        line = line.substr(endPos, string::npos);
    }
    else
        line.clear();

    return word;
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

