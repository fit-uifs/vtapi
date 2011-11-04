/* 
 * File:   Sequence.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:53
 */

#include "vtapi.h"

Sequence::Sequence(const KeyValues& orig, const String& name) : KeyValues(orig) {
    thisClass = "Sequence";

    if (!name.empty()) this->sequence = name;

    select = new Select(orig);
    select->from("sequences", "*");
    select->whereString("seqname", this->sequence);
    // res = PQexecf(connector->getConn(), String("SELECT * FROM "+ getString("dsname") +".sequences;").c_str());
}

bool Sequence::next() {
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

Interval* Sequence::newInterval(const int t1, const int t2) {
    return new Interval(*this);
}


bool Sequence::add(String name, String location) {
    destruct(insert);

    insert = new Insert(*this, "sequences");
    insert->keyString("seqname", name);
    insert->keyString("seqlocation", location);
    // that's all, folks ... continue similarly if needed
}