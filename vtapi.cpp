/* 
 * File:   VTApi.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:42
 */

#include "VTApi.h"

VTApi::VTApi(const String& connStr) : Commons(connStr) {
};

VTApi::VTApi(const VTApi& orig) : Commons(orig) {
}

VTApi::VTApi(const Commons& orig) : Commons(orig) {
}

VTApi::~VTApi() {
}


Dataset* VTApi::newDataset() {
    return (new Dataset(*this));
};

Dataset* VTApi::newDataset(String name) {
    // TODO: this
};

