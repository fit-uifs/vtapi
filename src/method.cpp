/**
 * @file    method.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Method class
 */

#include "data/vtapi_method.h"

using namespace vtapi;


Method::Method(const KeyValues& orig, const string& name) : KeyValues(orig) {
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

string Method::getName() {
    return this->getString("mtname");
}

Process* Method::newProcess(const string& name) {
    return (new Process(*this, name));
}

TKeys Method::getMethodKeys() {
    if (methodKeys.empty()) {
        KeyValues* kv = new KeyValues(*this);
        kv->select = new Select(*this);
        kv->select->from("public.methods_keys", "keyname");
        kv->select->from("public.methods_keys", "typname");
        kv->select->from("public.methods_keys", "inout");
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
    }
    return methodKeys;
}

bool Method::run() { return VT_OK; };
