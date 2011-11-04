/* 
 * File:   main.cpp
 * Author: Vojtěch Fröml
 *
 * Created on October 9, 2011, 6:58 PM
 */

#include <cstdlib>
#include <iostream>

#include "vtapi.h"
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

