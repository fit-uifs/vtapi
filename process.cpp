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


Process::Process(const KeyValues& orig, const String& name) : KeyValues(orig) {
    thisClass = "Process";

    String s = "SELECT P.*, PA1.relname AS inputs, PA2.relname AS outputs\n"
            "  FROM public.processes P\n"
            "  LEFT JOIN pg_catalog.pg_class PA1 ON P.inputs::regclass = PA1.relfilenode\n"
            "  LEFT JOIN pg_catalog.pg_class PA2 ON P.outputs::regclass = PA2.relfilenode";
    if (!name.empty())
        s += "  WHERE P.prsname='" + name + "'";
    s += ";";

    select = new Select(orig, s.c_str());
}


bool Process::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        process = this->getName();
        selection = this->getOutputs();
    }

    return kv;
}


String Process::getName() {
    return this->getString("prsname");
}

String Process::getInputs() {
    return this->getString("inputs");
}

String Process::getOutputs() {
    return this->getString("outputs");
}

Interval* Process::newInterval(const int t1, const int t2) {
    return new Interval(*this);
}

