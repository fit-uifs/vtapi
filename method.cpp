/*
 * File:   method.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:54
 */

#include "vtapi.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;



Method::Method(const KeyValues& orig, const String& name) : KeyValues(orig) {
    // TODO: methodkeys = new TKeys(orig);
    
    select = new Select(*this);
    select->from("public.methods", "*");
    select->whereString("mtname", name);
}

Method::~Method() {
}

bool Method::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->method = this->getName();
    }

    return kv;
}

String Method::getName() {
    return this->getString("mtname");
}

/**
 * Create new sequence for current dataset
 * @return pointer to new sequence
 */
Process* Method::newProcess(const String& name) {
    return (new Process(*this, name));
}

std::vector<TKey> Method::getMethodKeys() {
    /*
    KeyValues* kv = new KeyValues(*this);
        kv->select = new Select(*this);
        kv->select->from("pg_catalog.pg_type", "oid, typname");

        while (kv->next()) {
            this->typemap->insert(kv->getOid("oid"), kv->getName("typname"));
        }

        this->typemap->dataloaded = true;

        delete kv;*/
    methodKeys.clear();

    KeyValues* kv = new KeyValues(*this);
    kv->select = new Select(*this);
    kv->select->from("methods_keys", "keyname, typname::regtype::oid, inout");
    kv->select->whereString("mtname", this->method);

    while (kv->next()) {
        TKey* mk = new TKey();
        mk->key = kv->getString("keyname");
        mk->type = kv->getString("typname");
        mk->size = 0;
        mk->from = kv->getString("inout");

        methodKeys.push_back(*mk);
    }

    return methodKeys;
}
