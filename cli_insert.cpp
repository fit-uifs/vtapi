/*
 * File:   insert.cpp
 * Author: Vojtěch Fröml
 *
 * Created on October 20, 2011, 14:05 PM
 */

#include "vtapi.h"
#include <time.h>
#include <cstdlib>
#include <iostream>

using namespace std;

CLIInsert::CLIInsert(Commons& orig){
    this->connector = (&orig)->getConnector();
    this->dataset = (&orig)->getDataset();
    this->type = GENERIC;
}
CLIInsert::~CLIInsert() {
    this->params.clear();
}

void CLIInsert::setType(InsertType newtype){
    this->type = newtype;
}

int CLIInsert::addParam(pair<String,String> param) {
    pair<map<String,String>::iterator,bool> ret;

    if (!param.first.empty() && !param.second.empty()) {
	ret = this->params.insert(param);
        // key already existed
        if (!ret.second) return -1;
        else return 0;
    }
    else return -1;
}

int CLIInsert::addParam(String key, String value) {
    return this->addParam(pair<String, String> (key, value));
}

String CLIInsert::getParam(String pname) {
    return this->params[pname];
}

void CLIInsert::getIntArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = string::npos;
    int tag;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConn());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        tag = atoi(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%int4", tag);
        if (endPos == string::npos) break;
        startPos = endPos + 1;
    }
}

void CLIInsert::getFloatArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = string::npos;
    float svm;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConn());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        svm = (float) atof(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%float4", svm);
        if (endPos == string::npos) break;
        startPos = endPos + 1;
    }
}

bool CLIInsert::checkLocation(String seqname, String intlocation) {

    String location;
    PGresult* res;
    PGtext seqlocation = (PGtext) "";

    // get sequence location
    res = PQexec(this->connector->getConn(),
        String("SELECT seqlocation FROM " + this->dataset +
            ".sequences WHERE seqname=\'" + seqname.c_str() + "\';").c_str());
    if(!res) {
        this->connector->getLogger()->write(PQgeterror());
        return false;
    }
    PQgetf(res, 0, "%varchar", 0, &seqlocation);

    // FIXME: check location using getDataLocation();
    location = this->dataset.append((String) seqlocation).append(intlocation);
    // TODO:
    
    PQclear(res);
    return true;
    
}

bool CLIInsert::execute() {

    stringstream query;
    PGresult *res;
    PGarray arr;
    PGparam *param = PQparamCreate(this->connector->getConn());
    bool queryOK = true;
    PGtimestamp timestamp = getTimestamp();

    if (this->type == GENERIC) {
        this->connector->getLogger()->write("Error: insert command incomplete\n");
        return 0;
    }

    //TODO: datasets/methods/selections
    // insert sequence
    if (this->type == SEQUENCE) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%int4", atoi(getParam("seqnum").c_str()));
        PQputf(param, "%varchar", getParam("location").c_str());
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".sequences " <<
            "(seqname, seqnum, seqlocation, seqtyp, created) " <<
            "VALUES ($1, $2, $3, \'" << getParam("seqtype") << "\', $4)";
    }
    // insert interval
    else if (this->type == INTERVAL) {
        PQputf(param, "%name", getParam("sequence").c_str());
        PQputf(param, "%int4", atoi(getParam("t1").c_str()));
        PQputf(param, "%int4", atoi(getParam("t2").c_str()));
        PQputf(param, "%varchar", getParam("location").c_str());
        getIntArray(getParam("tags"), &arr);
        PQputf(param, "%int4[]", &arr);
        PQparamReset(arr.param);
        getFloatArray(getParam("svm"), &arr);
        PQputf(param, "%float4[]", &arr);
        PQparamClear(arr.param);
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".intervals " <<
            "(seqname, t1, t2, imglocation, tags, svm, created) " <<
            "VALUES ($1, $2, $3, $4, $5, $6, $7)";
        queryOK = checkLocation(getParam("sequence"), getParam("location"));
    }
    // insert process
    if (this->type == PROCESS) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%name", atoi(getParam("method").c_str()));
        PQputf(param, "%varchar", getParam("inputs").c_str());
        PQputf(param, "%varchar", getParam("outputs").c_str());
        PQputf(param, "%timestamp", &timestamp);
        query << "INSERT INTO " << this->dataset << ".processes " <<
            "(prsname, mtname, inputs, outputs, created) VALUES ($1, $2, $3, $4, $5)";
    }

    if (queryOK) {
        res = PQparamExec(this->connector->getConn(), param, query.str().c_str(), 1);
        if(!res)
            this->connector->getLogger()->write(PQgeterror());
        else PQclear(res);
    }

    PQparamClear(param);

    return 1;
}

void CLIInsert::clear(){

    this->params.clear();
    this->type = GENERIC;
}

PGtimestamp CLIInsert::getTimestamp() {
    PGtimestamp timestamp;
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    timestamp.date.isbc  = 0;
    timestamp.date.year  = timeinfo->tm_year + 1900;
    timestamp.date.mon   = timeinfo->tm_mon;
    timestamp.date.mday  = timeinfo->tm_mday;
    timestamp.time.hour  = timeinfo->tm_hour;
    timestamp.time.min   = timeinfo->tm_min;
    timestamp.time.sec   = timeinfo->tm_sec;
    timestamp.time.usec  = 0;
    return timestamp;
}

