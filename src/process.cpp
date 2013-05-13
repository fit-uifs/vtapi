/**
 * @file    process.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Process class
 */

#include "data/vtapi_process.h"

using namespace vtapi;


Process::Process(const KeyValues& orig, const string& name) : KeyValues(orig) {
    thisClass = "Process";

    string query;

    if (BackendFactory::backend == POSTGRES) {
        query = "\nSELECT P.*, PA1.relname AS inputs, PA2.relname AS outputs\n"
                "  FROM public.processes P\n"
                "  LEFT JOIN pg_catalog.pg_class PA1 ON P.inputs::regclass = PA1.relfilenode\n"
                "  LEFT JOIN pg_catalog.pg_class PA2 ON P.outputs::regclass = PA2.relfilenode";
    }
    else {
        query = "\nSELECT * from public.processes";
    }

    if (!name.empty()) {
        query += "  WHERE P.prsname='" + name + "'";
        this->process = name;
    }
    query += ";";
    this->select = new Select(orig, query.c_str());
}


bool Process::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        process = this->getName();
        selection = this->getOutputs();
    }

    return kv;
}


string Process::getName() {
    return this->getString("prsname");
}

string Process::getInputs() {
    return this->getString("inputs");
}

string Process::getOutputs() {
    return this->getString("outputs");
}


bool Process::add(const string& method, const string& name, const string& selection) {
    bool ret = false;

    destruct(insert);
    insert = new Insert(*this, "processes");
    insert->keyString("mtname", method);
    insert->keyString("prsname", name);
    insert->keyString("outputs", selection);

    // this is the fun
    if (insert->execute()) {

        update = new Update(*this, "ALTER TABLE \""+ selection +"\" ADD COLUMN \""+ name +"\" real[];");
        ret = update->execute();
    }

    destruct(insert);
    destruct(update);
    return ret;
}


Interval* Process::newInterval(const int t1, const int t2) {
    return new Interval(*this);
}

