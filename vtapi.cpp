/* 
 * File:   VTApi.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:42
 */

#include <cstdlib>
#include <iostream>
#include <time.h>

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

    // TODO: fill commons with args (logger, connector, user, password, location)
    //  eventually dataset/sequence/... from cmdline
    commons = new Commons(args_info);

    // TODO: free args_info somewhere
    // cmdline_parser_free (&args_info);
    free (cli_params);
}

VTApi::~VTApi() {
}


Dataset* VTApi::newDataset(const String& name) {
    if (name.empty()) return (new Dataset(*commons));
    // TODO: else
};

String VTApi::getWord(String& line, const String& prefix) {
    String word;
    size_t pos, nextPos = string::npos;

    if (line.empty()) return "";
    // TODO: zadani slova v uvozovkach?
    
    if (!prefix.empty()) {
        if (line.find(prefix, 0) == 0)
            line = line.substr(prefix.length(), string::npos);
        else return "";
    }
    pos = line.find_first_of(" \t\n");
    word = line.substr(0, pos);

    if (pos != string::npos)
        nextPos = line.find_first_not_of(" \t\n", pos);
    if (nextPos != string::npos)
        {

            line = line.substr(nextPos,string::npos);
        }
    else line.clear();
    return word;

}

int VTApi::run() {


    Dataset* dataset = newDataset();
    Sequence* sequence = dataset->newSequence();
    Process* process = new Process(*dataset);

    String line, command;

    cout << "commands: query, select, insert, update, delete, show, test, exit" << endl;

    while (1) {
        cout << "vtapi> ";
        getline(cin, line);
        command = getWord(line, "");

        // exit
        if (command.compare("exit") == 0) break;
        //general query
        else if (command.compare("query") == 0) {
            //TODO: where to execute general query
            PGresult* res;
            PQprintOpt opt = {0};

            res = PQexecf(commons->getConnector()->getConnection(), line.c_str());

            opt.header    = 1;
            opt.align     = 1;
            opt.fieldSep  = "|";
            PQprint(stdout, res, &opt);

            if(!res)
                    cout << "ERROR: " << PQgeterror() << endl;
            PQclear(res);
            
        }
        // dataset | sequence | interval | method | process | selection
        // data specific command (select, insert..)
        // insert dataset ... loc=..
        // insert sequence ... num=.. [loc=.. type=..]
        // insert interval into ... t1=.. t2=.. [loc= tags=.. svm=..]
        // insert method ... --
        // insert process ... method=.. in=.. out=..
        // insert selection ... --
        else {


        if (command.compare("select") == 0) {
            String what;
            what = getWord(line, "");
            cout << "todo" << endl;
            //if (what.compare("sequence")) dataset->select

        }

        // TODO: insert class?
        // TODO: set all with PQPutf (to do in keyvalues)
        // TODO: insert into which dataset?
        else if (command.compare("insert") == 0) {

            String input, name;

            PGtimestamp created;
//            time_t rawtime;
//            struct tm * timeinfo;
//
//            time ( &rawtime );
//            timeinfo = localtime ( &rawtime );
//
//            created.date.isbc  = 0;
//            created.date.year  = timeinfo->tm_year;
//            created.date.mon   = timeinfo->tm_mon;
//            created.date.mday  = timeinfo->tm_mday;
//            created.time.hour  = timeinfo->tm_hour;
//            created.time.min   = timeinfo->tm_min;
//            created.time.sec   = timeinfo->tm_sec;
//            created.time.usec  = 0;

            input = getWord(line, "");


            // insert sequence
            if (input.compare("sequence") == 0) {
                String location, type, seqnum;
                name = getWord(line, "");
                seqnum = getWord(line,"num=");
                location = getWord(line, "loc=");
                type = getWord(line, "type=");
//
//                PGregisterType seqtype = {"seqtype=public.seqtype", NULL, NULL};
//                PQregisterTypes(commons->getConnector()->getConnection(), PQT_USERDEFINED, &seqtype, 1, 0);
                // TODO: how get type seqtype, unique number, timestamp

                PGresult *res = PQexecf(commons->getConnector()->getConnection(),
                    "INSERT INTO public.sequences (seqname, seqlocation, seqtyp, seqnum) "
                        "VALUES (%name, %varchar, \'images\', %int4)",
                    name.c_str(), location.c_str(), atoi(seqnum.c_str()));
                if(!res)
                    cout << "ERROR: " << PQgeterror() << endl;
                PQclear(res);


            }
            // insert interval
            if (input.compare("interval") == 0) {
                String into, location, t1, t2;
                getWord(line, "into");
                into = getWord(line, "");
                t1 = getWord(line, "t1=");
                t2 = getWord(line, "t2=");
                location = getWord(line, "loc=");

                // TODO: other columns
                PGresult *res = PQexecf(commons->getConnector()->getConnection(),
                    "INSERT INTO public.intervals (seqname, t1, t2, imglocation) "
                        "VALUES (%name, %int4, %int4, %varchar)",
                    into.c_str(), atoi(t1.c_str()), atoi(t2.c_str()), location.c_str());
                if(!res)
                    cout << "ERROR: " << PQgeterror() << endl;
                PQclear(res);


            }
            // insert process
            if (input.compare("process") == 0) {
                String mtname, in, out;
                name = getWord(line, "");
                getWord(line, "of");
                mtname = getWord(line, "");
                in = getWord(line, "in=");
                out = getWord(line, "out=");

//                PGregisterType regclass = {"regclass=pg_catalog.regclass", NULL, NULL};
//                PQregisterTypes(commons->getConnector()->getConnection(), PQT_USERDEFINED, &regclass, 1, 0);

                // TODO: get type regclass
                PGresult *res = PQexecf(commons->getConnector()->getConnection(),
                    "INSERT INTO public.processes (prsname, mtname, inputs, outputs) "
                        "VALUES (%name, %name, %varchar, %varchar)",
                    name.c_str(), mtname.c_str(), in.c_str(), out.c_str());
                if(!res)
                    cout << "ERROR: " << PQgeterror() << endl;
                PQclear(res);


            }


        }
        else if (command.compare("update") == 0) cout << "todo" << endl;
        else if (command.compare("delete") == 0) cout << "todo" << endl;
        else if (command.compare("show") == 0) cout << "todo" << endl;
        else if (command.compare("test") == 0) {

            Dataset* testDataset = newDataset();
            Test* test = new Test(*testDataset);
            test->testAll();
            delete test;
            delete testDataset;
        }
        else cout << "commands: query, select, insert,"
                "update, delete, show, test, exit" << endl;
        }

    }


    /* Deallocate memory */
    delete process;
    delete sequence;
    delete dataset;


    return 0;
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

