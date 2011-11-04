/*
 * File:   typemap.cpp
 * Author: Volf Tomas
 *
 * Created on 29. září 2011, 10:54
 */

#include "vtapi_commons.h"

TypeMap::TypeMap() {
    this->dataloaded = false;
}

TypeMap::~TypeMap() {
}

void TypeMap::clear() {
    oid2typname.clear();
    typname2oid.clear();
}

bool TypeMap::empty() {
    bool isEmpty = this->oid2typname.empty() || this->typname2oid.empty();
    if (isEmpty) {
        this->oid2typname.clear();
        this->typname2oid.clear();
    }
    return isEmpty;
}

void TypeMap::insert(int oid, String typname) {
    this->oid2typname[oid] = typname;
    this->typname2oid[typname] = oid;
}

void TypeMap::insert(String typname, int oid) {
    this->insert(oid, typname);
}

int TypeMap::size() {
    return this->oid2typname.size();
}

int TypeMap::toOid(const String typname) {
    return this->typname2oid[typname];
}

String TypeMap::toTypname(const int oid) {
    return this->oid2typname[oid];
}
