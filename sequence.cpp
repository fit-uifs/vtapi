/* 
 * File:   Sequence.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:53
 */

#include "vtapi.h"

Sequence::Sequence(const KeyValues& orig, const String& name) : KeyValues(orig) {
    if (!name.empty()) this->sequence = name;

    select = new Select(*this);
    select->from("sequences", "*");
    select->whereString("seqname", this->sequence);
    // res = PQexecf(connector->getConn(), String("SELECT * FROM "+ getString("dsname") +".sequences;").c_str());
}

Sequence::~Sequence() {
}

KeyValues* Sequence::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->sequence = this->getName();
        this->sequenceLocation = this->getLocation();
    }

    return kv;
}


String Sequence::getName() {
    return this->getString("seqname");
}

String Sequence::getLocation() {
    return this->getString("seqlocation");
}

Interval* Sequence::newInterval() {
    return new Interval(*this);
}


bool Sequence::add(String name) {
    destruct(insert);

    insert = new Insert(*this, "sequences");
    insert->valueString("seqname", name);
/*
    PGparam *param = PQparamCreate(connector->conn);

    // Pack one or more parameters into a PGparam.
    PQputf(param, "%varchar", name.c_str());
    PQputf(param, "%varchar", "nejake/");

    // Execute a parameterized query.
    PGresult *res = PQparamExec(connector->conn, param,
      "INSERT INTO sequences(seqname, location) VALUES ($1, $2)", PGF);


 */
}