/*
 * File:   typemap.cpp
 * Author: Volf Tomas
 *
 * Created on 29. září 2011, 10:54
 */

#include "vtapi_commons.h"

using namespace std;

TypeMap::TypeMap(Connector* connector) {
    this->connector = connector;
    dataloaded = false;
}

TypeMap::~TypeMap() {
    clear();
}

void TypeMap::clear() {
    typesname.clear();
    typesoid.clear();
    reftables.clear();
    dataloaded = false;
}

bool TypeMap::empty() {
    bool isEmpty = this->typesname.empty() || this->typesoid.empty();
    if (isEmpty) this->clear();
    return isEmpty;
}

//void TypeMap::insert(int oid, String typname) {
//    this->oid2typname[oid] = typname;
//    this->typname2oid[typname] = oid;
//}

int TypeMap::size() {
    return typesoid.size();
}

int TypeMap::toOid(const String typname) {
    loadTypes();
    if (!typesname.count(typname) > 0) return -1;
    return this->typesname[typname].first;
}

String TypeMap::toTypname(const int oid) {
    loadTypes();
    if (!typesoid.count(oid) > 0) return "";
    return this->typesoid[oid].first;
}

void TypeMap::loadTypes() {
    if (!dataloaded) {
        PGresult* res = PQexec(connector->getConn(),
            "SELECT oid, typname, typcategory, typlen, typelem from pg_catalog.pg_type");
        // load data types
        for (int i = 0; i < PQntuples(res); i++) {
            struct typeinfo ti;
            PGint4 oid;
            PGtext typname;
            PQgetf(res, i, "%oid %name %char %int2 %oid",
                0, &oid, 1, &typname, 2, &ti.category, 3, &ti.length, 4, &ti.elemoid);
            typesname[String(typname)] = make_pair(oid, ti);
            typesoid[oid] = make_pair(String(typname), ti);
        }
        // load defined reference types
        loadRefTypes();
        dataloaded = true;
        PQclear(res);
    }
}
void TypeMap::loadRefTypes() {
    reftables["regproc"] = make_pair("pg_catalog.pg_proc", "proname");
    reftables["regprocedure"] = make_pair("pg_catalog.pg_proc", "proname");
    reftables["regoper"] = make_pair("pg_catalog.pg_operator", "oprname");
    reftables["regoperator"] = make_pair("pg_catalog.pg_operator", "oprname");
    reftables["regtype"] = make_pair("pg_catalog.pg_type", "typname");
    reftables["regclass"] = make_pair("pg_catalog.pg_class", "relname");
}
bool TypeMap::isRefType(String name) {
    loadTypes();
    return (reftables.count(name) > 0);
}
bool TypeMap::isEnumType(String name) {
    loadTypes();
    return (typesname.count(name) > 0) ? (typesname[name].second.category == 'E') : false;
}
pair<String,String> TypeMap::getRefTable(String name) {
    loadTypes();
    return (reftables.count(name) > 0) ? reftables[name] : make_pair(String(""),String(""));
}
char TypeMap::getCategory (String name) {
    loadTypes();
    return typesname[name].second.category;
}
char TypeMap::getCategory (int oid) {
    loadTypes();
    return typesoid[oid].second.category;
}
short TypeMap::getLength (String name) {
    loadTypes();
    return typesname[name].second.length;
}
short TypeMap::getLength (int oid) {
    loadTypes();
    return typesoid[oid].second.length;
}
int TypeMap::getElemOID (String name) {
    loadTypes();
    return typesname[name].second.elemoid;
}
int TypeMap::getElemOID (int oid) {
    loadTypes();
    return typesoid[oid].second.elemoid;
}