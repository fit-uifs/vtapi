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

    if (!name.empty()) this->process = name;

    select = new Select(orig);
    select->from("processes", "*");
    select->whereString("prsname", this->process);

//    String s = "SELECT P.*, PA1.relname AS inputs, PA2.relname AS outputs\n"
//            "  FROM public.processes P\n"
//            "  LEFT JOIN pg_catalog.pg_class PA1 ON P.inputs::regclass = PA1.relfilenode\n"
//            "  LEFT JOIN pg_catalog.pg_class PA2 ON P.outputs::regclass = PA2.relfilenode";
//    if (!name.empty())
//        s += "  WHERE P.prsname='" + name + "'";
//    s += ";";
//
//    select = new Select(orig, s.c_str());
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


bool Process::add(const String& method, const String& name, const String& selection) {
    destruct(insert);

    insert = new Insert(*this, "processes");
    insert->keyString("mtname", method);
    insert->keyString("prsname", name);
    insert->keyString("outputs", selection);

    // this is the fun
    if (insert->execute()) {
        update = new Update(*this, "ALTER TABLE \""+ selection +"\" ADD COLUMN \""+ name +"\" real[];");
        bool ok = update->execute();
        return ok;
    }

    destruct(insert);
    destruct(update);
    return false;
}


Interval* Process::newInterval(const int t1, const int t2) {
    return new Interval(*this);
}

