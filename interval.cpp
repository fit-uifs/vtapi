/* 
 * File:   Interval.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:54
 */

#include "VTApi.h"

Interval::Interval(const KeyValues& other) : KeyValues(other) {
    res = PQexecf(getConnector()->getConnection(), String("SELECT * FROM public.intervals;").c_str());
}

Interval::Interval(const Interval& orig) : KeyValues(orig) {
}

Interval::~Interval() {
}

String Interval::getSequence() {
    return this->getString("seqname");
}

String Interval::getLocation() {
    return this->getString("imglocation");
}

int Interval::getStartTime() {
    return this->getInt("t1");
}

int Interval::getEndTime() {
    return this->getInt("t2");
}
