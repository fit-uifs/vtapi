/* 
 * File:   Sequence.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:53
 */

#include "VTApi.h"

Sequence::Sequence(const KeyValues& other) : KeyValues(other) {
}

Sequence::Sequence(const Sequence& orig) : KeyValues(orig) {
}

Sequence::~Sequence() {
}

