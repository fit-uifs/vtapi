/* 
 * File:   VTCli.cpp
 * Author: vojca
 * 
 * Created on November 3, 2011, 7:12 PM
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include "vtcli.h"
#include "cli_settings.h"


using namespace std;

VTCli::VTCli(int argc, char** argv){
    this->processArgs(argc, argv);
    this->vtapi = new VTApi(argc, argv);
}
VTCli::VTCli(const VTApi& api) {
    this->vtapi = new VTApi(api);
}

VTCli::~VTCli() {
    delete(vtapi);
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
                cout << "helping you with query command" << endl;
                continue;
            }
            if (line.empty()) continue;
            Query* query = new Query(*(this->vtapi->commons), line, NULL);
            query->execute();
            (this->vtapi->commons)->printRes(query->res);
            delete(query);
        }
        // command: select
        else if (!command.compare("select")) {
            String input = getWord(line);
            // select help
            if (!input.compare("help")) {
                cout << this->helpStrings["select"];
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
                delete(ds);
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
                delete(seq);
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
                delete(in);
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
                delete(pr);
            }
            // select method
            else if (!input.compare("method")) {
                Method* me = new Method(*(this->vtapi->commons));
                //TODO: methodkeys not implemented yet
                me->select->where.clear();
                me->select->whereString("mtname", params["name"]);
                me->next();
                me->printAll();
                delete(me);
            }
        }
        // command: insert
        else if (command.compare("insert") == 0) {
            String input = getWord(line);
            if (!input.compare("help")) {
                cout << this->helpStrings["insert"];
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
            delete(insert);
        }
        // TODO: update
        else if (!command.compare("update")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                cout << this->helpStrings["update"];
                continue;
            }
            cout << "todo update" << endl;
        }
        // TODO: delete
        else if (!command.compare("delete")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                cout << this->helpStrings["delete"];
                continue;
            }
            cout << "todo delete" << endl;
        }
        // TODO: show
        else if (!command.compare("show")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                cout << this->helpStrings["show"];
                continue;
            }
            cout << "todo show" << endl;
        }
        // command: test
        else if (!command.compare("test")) {
            this->vtapi->test();
        }
        // command: help
        else if (!command.compare("help")) {
            cout << this->helpStrings["all"];
        }
        else this->vtapi->commons->warning("Unknown command");

    } while(interact);

    return 0;
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