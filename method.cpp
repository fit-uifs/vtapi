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



Method::Method(const KeyValues& orig) : KeyValues(orig) {
    // TODO: methodkeys = new TKeys(orig);
    
    select = new Select(*this);
    select->from("public.methods", "*");
}

Method::~Method() {
}

String Method::getMtname() {
    return this->getString("mtname");
}

