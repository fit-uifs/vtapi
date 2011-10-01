/* 
 * File:   dataset.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:52
 */

#include "VTApi.h"

Dataset::Dataset(const KeyValues& other) : KeyValues(other) {
}

Dataset::Dataset(const Dataset& orig) : KeyValues(orig) {
}

Dataset::~Dataset() {
}

