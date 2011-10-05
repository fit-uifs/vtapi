/* 
 * File:   dataset.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:52
 */

#include "VTApi.h"

Dataset::Dataset(const KeyValues& orig) : KeyValues(orig) {
    // FIXME: tohle by se melo poslat KeyValues, at si to zpracuji
    res = PQexecf(getConnector()->getConnection(), "SELECT * FROM public.datasets;");
}

Dataset::Dataset(const Dataset& orig) : KeyValues(orig) {
    // TODO: rozhodnout jak se to tu udela
}

Dataset::~Dataset() {
}

String Dataset::getName() {
    PGtext text;                        // char* .. not necessarily initialized
    if(res) {
        PQgetf(res, position, "#text", "dsname", &text); // get formated text value from tuple at position
    }
    return String(text);
}

String Dataset::getLocation() {
    PGtext text;                        // char* .. not necessarily initialized
    if(res) {
        PQgetf(res, position, "#text", "dslocation", &text); // get formated text value from tuple at position
    }
    return String(text);
}

Sequence* Dataset::newSequence() {
    return (new Sequence(*this));
}
