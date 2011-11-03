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

#include "vtcli.h"

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

    VTCli* vtcli = new VTCli(argc, argv);

    vtcli->run();

    delete vtcli;

    return 0;
}

