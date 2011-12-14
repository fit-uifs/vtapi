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
    // TODO: je nutne definovat chovani, kdyz tam typ neni!
    if (!dataloaded) loadTypes();
    return this->typesname[typname].first;
}

String TypeMap::toTypname(const int oid) {
    // TODO: je nutne definovat chovani, kdyz tam typ neni!
    if (!dataloaded) loadTypes();
    return this->typesoid[oid].first;
}

void TypeMap::loadTypes() {
    if (!dataloaded) {
        PGresult* res = PQexec(connector->getConn(),
            "SELECT oid, typname, typcategory, typlen, typelem from pg_catalog.pg_type");

        for (int i = 0; i < PQntuples(res); i++) {
            struct typeinfo ti;
            PGint4 oid;
            PGtext typname;
            PQgetf(res, i, "%oid %name %char %int2 %oid",
                0, &oid, 1, &typname, 2, &ti.category, 3, &ti.length, 4, &ti.elemoid);
            typesname[String(typname)] = make_pair(oid, ti);
            typesoid[oid] = make_pair(String(typname), ti);
            if (i == 3) cout << ti.category << ti.length << ti.elemoid << endl;
        }
        dataloaded = true;
        PQclear(res);
    }
}