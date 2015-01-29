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

#include <vtapi_global.h>
#include <data/vtapi_interval.h>
#include <data/vtapi_process.h>

#include "data/vtapi_method.h"

using namespace vtapi;


Process::Process(const KeyValues& orig, const string& name) : KeyValues(orig) {
    thisClass = "Process";

    string query;

    if (backend == POSTGRES) {
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
    bool retval = VT_OK;

    vt_destruct(insert);
    insert = new Insert(*this, "processes");
    retval &= insert->keyString("mtname", method);
    retval &= insert->keyString("prsname", name);
    retval &= insert->keyString("outputs", selection);
    if (retval) retval &= insert->execute();

    // this is the fun
    if (retval) {
        retval = this->prepareOutput(method, selection);
//        update = new Update(*this, "ALTER TABLE \""+ selection +"\" ADD COLUMN \""+ name +"\" real[];");
//        retval &= update->execute();
    }
    
    vt_destruct(insert);
//    vt_destruct(update);

    return retval;
}


Interval* Process::newInterval(const int t1, const int t2) {
    return new Interval(*this);
}

bool Process::prepareOutput(const string& method, const string& selection) {
    // podívej se, jestli ve výstupní tabulce "selection" je tento sloupec připraven
    // pokud ve výstupní tabulce sloupec není, vytvoř jej
    
    bool retval = true;

    Method* mth = new Method(*this, method);
    mth->next();
    TKeys methodKeys = mth->getMethodKeys();
    
    map<string,string> colsToAdd = this->diffColumns(selection, methodKeys);
    if (retval = colsToAdd.size()) {
      retval = this->addColumns(selection, colsToAdd);
    }
    
    vt_destruct(mth);
    
    return retval;
}

map<string,string> Process::diffColumns(const string& table, const TKeys& methodKeys) {
    map<string,string> tableCols;
    map<string,string>::iterator iTableCols;
    map<string,string> colsToAdd;
    
    KeyValues* kv = new KeyValues(*this);
    
//    kv->select = new Select(*this, "SELECT column_name, data_type FROM information_schema.columns WHERE table_schema = '" + this->dataset + "' AND table_name   = '" + table + "'");
    // TODO: At the moment Postgres ONLY):( 
    kv->select = new Select(*this, "SELECT a.attname AS column_name, a.atttypid::regtype::text AS data_type FROM pg_catalog.pg_attribute a JOIN pg_catalog.pg_class r ON r.oid = a.attrelid JOIN pg_catalog.pg_namespace n ON n.oid = r.relnamespace WHERE n.nspname = '" + this->dataset + "' AND r.relname = '" + table + "' AND attstattarget = -1;");
    while (kv->next()) {
        tableCols.insert(pair<string,string>(kv->getString("column_name"), kv->getString("data_type")));
    }
   
    for (TKeys::const_iterator it = methodKeys.begin(); it != methodKeys.end(); ++it) {
        if (it->from == "out" || it->from == "inout") {
            iTableCols = tableCols.find(it->key);
            if (iTableCols == tableCols.end()) {
                colsToAdd.insert(pair<string,string>(it->key, it->type));
            }
            else {
                if (iTableCols->second != it->type) {
                    this->logger->error(360, "VTApi panic: Column " + it->key + " exists, but there is not the same type (selection's attribute type: " + iTableCols->second + " X method's attribute type: " + it->type + ")!", this->thisClass + "diffColumns()");
                }
            }
        }
    }
    
    vt_destruct(kv->select);
    vt_destruct(kv);
    
    return colsToAdd;
}

bool Process::addColumns(const string& table, const map<string,string>& colsToAdd) {
    bool retval = true;
    
    if (colsToAdd.size()) {
        string queryString = "ALTER TABLE " + table + " ";
        for (map<string,string>::const_iterator it = colsToAdd.begin(); it != colsToAdd.end(); it++) {
            queryString += "ADD COLUMN \"" + it->first + "\" " + it->second + ", ";
        }
        queryString.resize(queryString.size() - 2);
        
        Query* query = new Query(*this, queryString + ";");
        retval = query->execute();
        
        vt_destruct(query);
    }
    return retval;
}