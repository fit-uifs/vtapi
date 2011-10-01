/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#include "VTApi.h"

KeyValues::KeyValues(const Commons& orig) : Commons(orig),
        position(-1), res(NULL), parent(NULL) {
}

KeyValues::KeyValues(const KeyValues& orig) : Commons(orig),
        position(-1), res(NULL), parent(NULL) {
        // FIXME: tohle taky ne: position(orig.position), res(orig.res), parent(orig.parent) {
}

KeyValues::~KeyValues() {
}

KeyValues* KeyValues::next() {
    // TODO: zatim to skonci po konci resultsetu, ale melo by zjistit, jestli je
    // to na konci nebo neni a spachat kdyztak dalsi dotaz (limit, offset)
    if (res && position < PQntuples(res)) position++;
}

String KeyValues::getString(String key) {
    // TODO: mno a tu zacina fun :)
    return String("public");
}

