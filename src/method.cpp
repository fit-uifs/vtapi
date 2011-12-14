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
    thisClass = "Method";
    
    select = new Select(orig);
    select->from("public.methods", "*");
    select->whereString("mtname", name);
}

bool Method::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->method = this->getName();
        this->getMethodKeys();
    }

    return kv;
}

String Method::getName() {
    return this->getString("mtname");
}

/**
 * Create new sequence for current dataset
 * @todo Opravdu sequence a ne náhodou process? Zkontrolovat výsledek ve vtapi.h! [TV]
 * @return pointer to new sequence
 */
Process* Method::newProcess(const String& name) {
    return (new Process(*this, name));
}

std::vector<TKey> Method::getMethodKeys() {
    methodKeys.clear();

    KeyValues* kv = new KeyValues(*this);
    kv->select = new Select(*this);
    kv->select->from("methods_keys", "keyname");
    kv->select->from("methods_keys", "typname");
    kv->select->from("methods_keys", "inout");
    kv->select->whereString("mtname", this->method);

    while (kv->next()) {
        TKey mk;
        mk.key = kv->getString("keyname");
        mk.type = kv->getString("typname");
        mk.size = 0;   // 0 is for the definition
        mk.from = kv->getString("inout");
        methodKeys.push_back(mk);
    }

    delete (kv);
    return methodKeys;
}
