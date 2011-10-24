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
 * The CLI main function
 * You can follow the
 *
 * @param argc
 * @param argv
 * @return sucess
 */
int main(int argc, char** argv) {

   
    VTApi* vtapi = new VTApi(argc, argv);

    vtapi->run();

    delete vtapi;

    return 0;
}

