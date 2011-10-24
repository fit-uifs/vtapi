/*
 * File:   insert.cpp
 * Author: Vojtěch Fröml
 *
 * Created on October 20, 2011, 14:05 PM
 */

#include "VTApi.h"
#include <time.h>
#include <cstdlib>
#include <iostream>

using namespace std;

Insert::Insert(Dataset* ds){
    this->connector = ds->getConnector();
    this->dataset = ds;
    this->type = NONE;
}
Insert::~Insert() {
    this->params.clear();
}

void Insert::setDataset(Dataset* newdataset) {
    this->dataset = newdataset;
}

void Insert::setType(InsertType newtype){
    this->type = newtype;
}

void Insert::addParam(String param){
    this->params.push_back(param);
}

String Insert::getParam(String pname) {

    // P3k: hmm... neni neco efektivnejsiho?
    pname.append("=");
    for (int i = 0; i < this->params.size(); i++) {
        if (this->params.at(i).find(pname) == 0) {
            return this->params.at(i).substr(pname.length(), string::npos);
            break;
        }
    }
    return "";
}

void Insert::getIntArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = string::npos;
    int tag;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConnection());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        tag = atoi(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%int4", tag);
        if (endPos == string::npos) break;
        startPos = endPos + 1;
    }
}

void Insert::getFloatArray(String arrayParam, PGarray* arr) {

    size_t startPos = 0, endPos = string::npos;
    float svm;
    arr->ndims = 0;
    arr->param = PQparamCreate(this->connector->getConnection());

    while (startPos < arrayParam.length()) {
        endPos = arrayParam.find(',', startPos);
        svm = (float) atof(arrayParam.substr(startPos, endPos).c_str());
        PQputf(arr->param, "%float4", svm);
        if (endPos == string::npos) break;
        startPos = endPos + 1;
    }
}

bool Insert::checkLocation(String seqname, String intlocation) {

    String location;
    PGresult* res;
    PGtext seqlocation = (PGtext) "";

    // get sequence location
    res = PQexec(this->connector->getConnection(),
        String("SELECT seqlocation FROM " + this->dataset->getName() +
            ".sequences WHERE seqname=\'" + seqname.c_str() + "\';").c_str());
    if(!res) {
        this->connector->getLogger()->write(PQgeterror());
        return false;
    }
    PQgetf(res, 0, "%varchar", 0, &seqlocation);

    // TODO: check location
    location = this->dataset->getLocation().append((String) seqlocation).append(intlocation);
    // check here
    
    PQclear(res);
    return true;
    
}

bool Insert::execute() {

    stringstream query;
    PGresult *res;
    PGarray arr;
    PGparam *param = PQparamCreate(this->connector->getConnection());
    bool queryOK = true;

    if (this->type == NONE) {
        this->connector->getLogger()->write("Error: insert command incomplete\n");
        return 0;
    }

    //TODO: datasets/methods/selections + timestamp
    // insert sequence
    if (this->type == SEQUENCE) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%int4", atoi(getParam("seqnum").c_str()));
        PQputf(param, "%varchar", getParam("location").c_str());
        query << "INSERT INTO " << this->dataset->getName() << ".sequences " <<
            "(seqname, seqnum, seqlocation, seqtyp) VALUES ($1, $2, $3, \'" <<
            getParam("seqtype") << "\')";       
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
        query << "INSERT INTO " << this->dataset->getName() << ".intervals " <<
            "(seqname, t1, t2, imglocation, tags, svm) VALUES ($1, $2, $3, $4, $5, $6)";
        queryOK = checkLocation(getParam("sequence"), getParam("location"));
    }
    // insert process
    if (this->type == PROCESS) {
        PQputf(param, "%name", getParam("name").c_str());
        PQputf(param, "%name", atoi(getParam("method").c_str()));
        PQputf(param, "%varchar", getParam("inputs").c_str());
        PQputf(param, "%varchar", getParam("outputs").c_str());
        query << "INSERT INTO " << this->dataset->getName() << ".processes " <<
            "(prsname, mtname, inputs, outputs) VALUES ($1, $2, $3, $4)";
    }

    if (queryOK) {
        res = PQparamExec(this->connector->getConnection(), param, query.str().c_str(), 1);
        if(!res)
            this->connector->getLogger()->write(PQgeterror());
        else PQclear(res);
    }

    PQparamClear(param);

    return 1;
}

void Insert::clear(){

    this->params.clear();
    this->type = NONE;
}

PGtimestamp Insert::getTimestamp() {
//    PGtimestamp created;
//    time_t rawtime;
//    struct tm * timeinfo;
//
//    time ( &rawtime );
//    timeinfo = localtime ( &rawtime );
//
//    created.date.isbc  = 0;
//    created.date.year  = timeinfo->tm_year;
//    created.date.mon   = timeinfo->tm_mon;
//    created.date.mday  = timeinfo->tm_mday;
//    created.time.hour  = timeinfo->tm_hour;
//    created.time.min   = timeinfo->tm_min;
//    created.time.sec   = timeinfo->tm_sec;
//    created.time.usec  = 0;
}

