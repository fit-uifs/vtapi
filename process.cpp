 /*
 * File:   process.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:54
 */

#include "vtapi.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;



Process::Process(const KeyValues& orig) : KeyValues(orig) {
    select = new Select(*this,
            "SELECT P.prsname, PA1.relname AS inputs, PA2.relname AS outputs "
            "  FROM public.processes P "
            "  LEFT JOIN pg_catalog.pg_class PA1 ON P.inputs::regclass = PA1.relfilenode "
            "  LEFT JOIN pg_catalog.pg_class PA2 ON P.outputs::regclass = PA2.relfilenode ");
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

