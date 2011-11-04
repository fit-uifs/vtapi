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

/**
 * Get name of current dataset
 * @return name name of current dataset
 */
String Dataset::getName() {
    return this->getString("dsname");
}

/**
 * Get location of current dataset
 * @return location of current dataset
 */
String Dataset::getLocation() {
    return this->getString("dslocation");
}

/**
 * Create new sequence for current dataset
 * @return pointer to new sequence
 */
Sequence* Dataset::newSequence(const String& name) {
    return (new Sequence(*this, name));
}

/**
 * Create new sequence for current dataset
 * @return pointer to new sequence
 */
Method* Dataset::newMethod(const String& name) {
    return (new Method(*this, name));
}

/**
 * Create new sequence for current dataset
 * @return pointer to new sequence
 */
Process* Dataset::newProcess(const String& name) {
    return (new Process(*this, name));
}