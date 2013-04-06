/* 
 * File:   dataset.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:52
 */

#include "vtapi.h"


Dataset::Dataset(const KeyValues& orig, const String& name) : KeyValues(orig) {
    thisClass = "Dataset";

    // set the dataset name
    if (!name.empty()) {
        dataset = name;
    }
    if (dataset.empty()) {
        warning(313, "No dataset specified");
    }

    select = new Select(orig);
    select->from("public.datasets", "*");
    select->whereString("dsname", this->dataset);
}

bool Dataset::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->dataset = this->getName();
        this->datasetLocation = this->getLocation();
    }

    return kv;
}


String Dataset::getName() {
    return this->getString("dsname");
}


String Dataset::getLocation() {
    return this->getString("dslocation");
}


Sequence* Dataset::newSequence(const String& name) {
    return (new Sequence(*this, name));
}

Video* Dataset::newVideo(const String& name) {
    return (new Video(*this, name));
}

Method* Dataset::newMethod(const String& name) {
    return (new Method(*this, name));
}

Process* Dataset::newProcess(const String& name) {
    return (new Process(*this, name));
}