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

    select = new Select(orig);
    select->from(this->selection, "*");
    select->whereString("seqname", this->sequence);

    // thi is to detect if derived from a Sequence (highly recommended)
    parentSequence = NULL;
    parentSequenceDoom = true;
    if (!orig.sequenceLocation.empty()) {
        parentSequence = (Sequence*)&orig;
        parentSequenceDoom = false;
    }
}



String Interval::getSequenceName() {
    // TODO: possibly empty... possible warning 332
    return this->sequence;
}


Sequence* Interval::getSequence() {
    if (parentSequence && this->sequence.compare(parentSequence->sequence) == 0) {
        return this->parentSequence;
    }
    else {  // TODO: This is not OK... should be a query
        this->error(333, "The use of getSequence() function is not recomended without using the Sequence first.");
        // TODO: doom it here && query for next one??? (just in case when needed)
    }

    return NULL;
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
        insert->keyString("imglocation", location);
    }
    // that's all, folks ... continue similarly if needed

    return true;
}


bool Interval::preSet() {
    destruct(update);

    update = new Update(*this, this->selection);
    update->whereString("seqname", sequence);
    update->whereInt("t1", this->getInt("t1"));
    update->whereInt("t2", this->getInt("t2"));

    return true;
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

String Image::getLocation() {
    return this->getString("imglocation");
}

String Image::getDataLocation() {
    return (((Commons*)this)->getDataLocation() + this->getLocation());
}
