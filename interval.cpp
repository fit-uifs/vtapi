/* 
 * File:   Interval.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:54
 */

#include "vtapi.h"

Interval::Interval(const KeyValues& orig, const String& selection) : KeyValues(orig) {
    if (!selection.empty()) this->selection = selection;

    select = new Select(*this);
    select->from(this->selection, "*");
    select->whereString("seqname", this->sequence);
    // res = PQexecf(getConnector()->getConn(), String("SELECT * FROM public.intervals;").c_str());
}

Interval::~Interval() {
}

String Interval::getSequence() {
    return this->getString("seqname");
}

int Interval::getStartTime() {
    return this->getInt("t1");
}

int Interval::getEndTime() {
    return this->getInt("t2");
}







Image::Image(const KeyValues& orig, const String& selection) : Interval(orig, selection) {
}

Image::~Image() {
}

int Image::getTime() {
    int t1 = this->getStartTime();
    int t2 = this->getEndTime();

    if (t1 != t2) {
        warning(3291, "This is not an Image (see output if verbose)");
        // if (verbose) this->print();
    }
    return t1;
}

String Image::getLocation() {

}