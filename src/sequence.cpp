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

Image* Sequence::newImage(const String& name) {
    Image* image = new Image(*this);
    image->select->whereString("imglocation", name);
    return image;
}


bool Sequence::add(const String& name, const String& location, const String& type) {
    bool retval = true;

    destruct(insert);
    insert = new Insert(*this, "sequences");
    retval &= insert->keyString("seqname", name);
    retval &= insert->keyString("seqlocation", location);
    retval &= insert->keySeqtype("seqtyp", type);
    return retval;
}

bool Sequence::add(const String& name, const String& location, const String& type,
    const String& userid, const String& groupid, const String& notes) {
    bool retval = this->add(name, location, type);
    retval &= insert->keyString("userid", userid);
    retval &= insert->keyString("groupid", groupid);
    retval &= insert->keyString("notes", notes);
    return retval;
}

bool Sequence::addExecute() {
    bool retval = true;

    if (this->insert) {
        time_t now;
        time(&now);
        retval &= insert->keyTimestamp("created", now);
        retval &= this->insert->execute();
    }
    else retval = false;
    return retval;
}
