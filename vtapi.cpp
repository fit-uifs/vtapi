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
    // Interactive mode
    interact = true;
}

VTApi::~VTApi() {
}


Dataset* VTApi::newDataset(const String& name) {
    return (new Dataset(*commons, name));
};



int VTApi::run() {
    // TODO: toto nechat?
    if (cmdline.empty()) {
        cout << "VTApi is running" << endl;
        cout << "Commands: query, select, insert, update, delete, show, test, help, exit" << endl;
    }
    String line, command;
    // command cycle
    while (interact) {
        // get command, if cli is empty then start interactive mode
        if (cmdline.empty()) getline(cin, line);
        else {
            line = cmdline;
            interact = false;
        }
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
            Query* query = new Query(*commons, line, NULL);
            query->execute();
            commons->printRes(query->res);
            delete(query);
        }
        // command: select
        else if (!command.compare("select")) {
            String input = getWord(line);
            // select help
            if (!input.compare("help")) {
                cout << "helping you with select command " << endl;
                continue;
            }
            // get select params
            std::map<String,String> params;
            while (!line.empty()) params.insert(createParam(getWord(line)));

            // TODO: udelat pres Tkey
            // select dataset
            if (!input.compare("dataset")) {
                Dataset* ds = new Dataset(*commons);
                ds->select->where.clear();
                ds->select->whereString("dsname", params["name"]);
                ds->select->whereString("dslocation", params["location"]);
                ds->next();
                ds->printAll();
                delete(ds);
            }
            // select sequence
            else if (!input.compare("sequence")) {
                Sequence* seq = new Sequence(*commons);
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
                Interval* in = new Interval(*commons);
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
                Process* pr = new Process(*commons);
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
                Method* me = new Method(*commons);
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
                cout << "helping you with insert command " << endl;
                continue;
            }

            CLIInsert* insert = new CLIInsert(*commons);
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
                cout << "helping you with update command " << endl;
                continue;
            }
            cout << "todo update" << endl;
        }
        // TODO: delete
        else if (!command.compare("delete")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                cout << "helping you with delete command " << endl;
                continue;
            }
            cout << "todo delete" << endl;
        }
        // TODO: show
        else if (!command.compare("show")) {
            String input = getWord(line);
            if (!input.compare("help")) {
                cout << "helping you with show command " << endl;
                continue;
            }
            cout << "todo show" << endl;
        }
        // command: test
        else if (!command.compare("test")) {
            this->test();
        }
        // command: help
        else if (!command.compare("help")) {
            cout << "helping you" << endl;
        }
        else commons->warning("Unknown command");
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
/**
 * Cuts first comma-separated value from string
 * @param word CSV string
 * @return first CSV
 */
String VTApi::getCSV(String& word) {
    String csv;
    size_t pos = word.find(",", 0);

    csv = word.substr(0, pos);
    if (pos != string::npos && pos+1 < word.length())
        word = word.substr(pos+1, string::npos);
    else word.clear();
    return csv;
}



void VTApi::test() {
    // lines starting with cout should be ignored :)
    
    cout << "TESTING generic classes..." << endl;
    TKeyValue<float> tkvf ("float", "number", 32156.7, "test");
    cout << "TKeyValue<float> tkvf (32156.7) ... typeid: " << typeid(tkvf).name() << endl;
    tkvf.print();

    cout << "std::vector<TKey*> ..." << endl;
    std::vector<TKey*> v;
    v.push_back(&tkvf);
    TKey* kv2 = new  TKeyValue<String> ("varchar", "string", "whatever", "test");
    v.push_back(kv2);
    
    char* chs[] = {"1", "2", "3", "ctyri", "5"};
    TKeyValue<char*> tki ("varchar[]", "array", chs, 5);
    v.push_back(&tki);
    for (int i = 0; i < v.size(); ++i) (v[i])->print();

    cout << "static_cast< TKeyValue<char*>* >(v[2]);" << endl;
    TKeyValue<char*>* tkic = static_cast< TKeyValue<char*>* >(v[2]);
    tkic->print();

    // dataset usw.
    cout << "DONE testing generic classes." << endl;
    cout << endl << endl;
    cout << "TESTING  Dataset..." << endl;

    Dataset* dataset = this->newDataset();
    dataset->next();
    dataset->printAll();

    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Sequence..." << endl;
    cout << "USING dataset " << dataset->getDataset() << endl;

    Sequence* sequence = dataset->newSequence();
    sequence->next();
    sequence->printRes(sequence->select->res); // equivalent to printAll()

    srand(time(NULL));
    String sn = "test_sequence" + toString(rand()%1000);
    cout << "ADDING Sequence " << sn << endl;
    sequence->add(sn, "/test_location");
    sequence->next();

    // FIXME: proc to tady pada????
    // delete (sequence);
    sequence = dataset->newSequence();
    sequence->next();   // this can execute and commit (a suicide)
    sequence->printAll();

    cout << "DELETING Sequence " << sn << endl;
    Query* query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".sequences WHERE seqname='" + sn + "';");
    cout << "OK:" << query->execute() << endl;
    delete (query);

    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Interval..." << endl;
    cout << "USING sequence " << sequence->getSequence() << endl;
    
    Interval* interval = sequence->newInterval();
    interval->select->limit = 10;
    interval->next();
    interval->print();
    
    // this has no effect outside ...
    int t1 = 100000 + rand()%1000;
    cout << "ADING Image on " << interval->getSequence() << " [" << t1 << ", " << t1 << "]" << endl;
    Image* image = new Image(*interval);
    image->add(interval->getSequence(), t1, "nosuchimage.jpg");
    // insert->execute() or next() must be called after inserting all voluntary fields such as
    // image->insert->keyFloat("sizeKB", 100.3);
    // image->insert->execute();
    delete (image);    // if not called the destructor raises a warning
    // ... except the database => do not fordet to delete where t1 > 99999 !!!!
    delete (interval);
    delete (sequence);

    // process usw.
    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Method..." << endl;

    Method* method = dataset->newMethod();
    method->next();
    method->printAll();

    cout << endl;
    cout << "TESTING  MethodKeys" << endl;
    cout << "USING method " << method->getName() << endl;
    cout << "Printing methodKeys (" << method->methodKeys.size() << ")" << endl;

    for (int i = 0; i < method->methodKeys.size(); i++) {
        method->methodKeys[i].print();
    }

    cout << "DONE." << endl;
    cout << endl;
    cout << "TESTING Process" << endl;

    Process* process = method->newProcess();
    process->next();
    process->printAll();

    delete (process);
    delete (method);
    cout << "DONE." << endl;

    delete (dataset);
    cout << "DONE ALL ... see warnings." << endl;
}
