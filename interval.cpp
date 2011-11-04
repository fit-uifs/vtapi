/* 
 * File:   Interval.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:54
 */

#include "vtapi.h"

Interval::Interval(const KeyValues& orig, const String& selection) : KeyValues(orig) {
    thisClass = "Interval";

    if (!selection.empty()) this->selection = selection;

    select = new Select(*this);
    select->from(this->selection, "*");
    select->whereString("seqname", this->sequence);
}

String Interval::getSequence() {
    return this->sequence;
}

int Interval::getStartTime() {
    return this->getInt("t1");
}

int Interval::getEndTime() {
    return this->getInt("t2");
}

bool Interval::add(const String& sequence, const int t1, const int t2, const String& location) {
    destruct(insert);
    int te2 = t2;
    if (t2 < 0) te2 = t1;

    insert = new Insert(*this, this->selection);
    insert->keyString("seqname", sequence);
    insert->keyInt("t1", t1);
    insert->keyInt("t2", t2);
    if (!location.empty()) {
        insert->keyString("imglocation", this->getDataLocation() + location);
    }
    // that's all, folks ... continue similarly if needed
}

String Interval::getLocation() {
    return this->getString("imglocation");
}

String Interval::getDataLocation() {
    return (((Commons*)this)->getDataLocation() + this->getLocation());
}


// ************************************************************************** //
Image::Image(const KeyValues& orig, const String& selection) : Interval(orig, selection) {
    thisClass = "Image";
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

bool Image::add(const String& sequence, const int t, const String& location) {
    ((Interval*)this)->add(sequence, t, t, location);
}
