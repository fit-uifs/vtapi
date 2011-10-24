/* 
 * File:   Sequence.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:53
 */

#include "VTApi.h"

Sequence::Sequence(const KeyValues& orig) : KeyValues(orig) {
    // FIXME: tohle by se melo poslat KeyValues, at si to zpracuji
    res = PQexecf(getConnector()->getConnection(), String("SELECT * FROM "+ getString("dsname") +".sequences;").c_str());
}

Sequence::Sequence(const Sequence& orig) : KeyValues(orig) {
}

Sequence::~Sequence() {
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