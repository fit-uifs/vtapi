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

    /* Initialize parser parameters structure */
    cli_params = cmdline_parser_params_create();
    /* Hold check for required arguments until config file is parsed */
    cli_params->check_required = 0;

    /* Parse cmdline first */
    if (cmdline_parser_ext (argc, argv, &args_info, cli_params) != 0) {
        cmdline_parser_free (&args_info);
        free (cli_params);
        exit(1);
    }

    /* Get the rest of arguments from config file, don't override cmdline */
    cli_params->initialize = 0;
    cli_params->override = 0;
    cli_params->check_required = 1;

    /* Parse config file */
    if (cmdline_parser_config_file
        (args_info.config_arg, &args_info, cli_params) != 0) {
        cmdline_parser_free (&args_info);
        free (cli_params);
        exit(1);
    }

    // TODO: fill commons with args (logger, connector, user, password, location)
    //  eventually dataset/sequence/... from cmdline
    commons = new Commons(args_info);
    

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

    String line, command;
    size_t pos;

    while (1) {
        getline(cin, line);
        pos = line.find(' ');
        command = line.substr(0, pos);

        if (command.compare("query") == 0) {
            //TODO: where to execute general query
            PGresult* res;
            res = PQexecf(commons->getConnector()->getConnection(), line.substr(pos,string::npos).c_str());

            PQprint(stdout, res, NULL);
            

            PQclear(res);
            
        }
            //TODO:
        else if (command.compare("select") == 0) cout << "todo" << endl;
        else if (command.compare("insert") == 0) cout << "todo" << endl;
        else if (command.compare("update") == 0) cout << "todo" << endl;
        else if (command.compare("delete") == 0) cout << "todo" << endl;
        else if (command.compare("show") == 0) cout << "todo" << endl;
        else if (command.compare("exit") == 0) break;

    }


    /* Deallocate memory */
    delete sequence;
    delete dataset;

    return 0;
}