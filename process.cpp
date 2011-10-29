 /*
 * File:   process.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:54
 */

#include "VTApi.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;



Process::Process(const Dataset& orig) : KeyValues(orig) {
/*
    res = PQexecf(getConnector()->getConn(), String(" SELECT P.prsname, PA1.relname AS inputs, PA2.relname AS outputs "
                                                            " FROM public.processes P "
                                                            " LEFT JOIN pg_catalog.pg_class PA1 ON P.inputs::regclass = PA1.relfilenode "
                                                            " LEFT JOIN pg_catalog.pg_class PA2 ON P.outputs::regclass = PA2.relfilenode "
                                                           ).c_str());
 */
}

Process::Process(const Process& orig) : KeyValues(orig) {
}

Process::~Process() {
}

String Process::getPrsname() {
    return this->getString("prsname");
}

String Process::getInputs() {
    return this->getString("inputs");
}

String Process::getOutputs() {
    return this->getString("outputs");
}

void Process::printProcesses() {
    cout << left;
    cout << setw(20) << "Process" << setw(20) << "Inputs" << setw(20)<< "Outputs" << endl;
    for (int i = 0; i < PQntuples(select->res); i++) {
        this->next();
        cout << setw(20) << this->getPrsname() << setw(20) << this->getInputs() << setw(20) << this->getOutputs() << endl;
    }
}
