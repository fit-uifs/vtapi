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