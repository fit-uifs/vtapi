/* 
 * File:   main.cpp
 * Author: Vojtěch Fröml
 *
 * Created on October 9, 2011, 6:58 PM
 */

#include <cstdlib>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "cli_settings.h"
#include "VTApi.h"

using namespace std;

/**
 * 
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char** argv) {

/*

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

    // New VTApi object
    VTApi* vtapi = new VTApi(args_info.connection_arg);
*/
    
    VTApi* vtapi = new VTApi(argc, argv);

    vtapi->run();

    delete vtapi;
/*
    cmdline_parser_free (&args_info);
    free (cli_params);
*/
    return 0;
}

