/* 
 * File:   dataset.cpp
 * Author: chmelarp
 * 
 * Created on 29. září 2011, 10:52
 */

#include "VTApi.h"


Dataset::Dataset(const KeyValues& orig, const String& name) : KeyValues(orig) {
    this->select = new Select(orig);
    this->select->from("public.datasets", "*");

    // set the dataset name
    if (!name.empty()) {
        this->dataset = name;
    }
    if (this->dataset.empty()) {
        this->logger->warning(303, "The dataset is not specified");
    }
    else {
        this->select->whereString("public.datasets", "dsname", this->dataset);
    }

    // res = PQexecf(getConnector()->getConnection(), "SELECT * FROM public.datasets;");
}

Dataset::Dataset(const Dataset& orig) : KeyValues(orig) {
    // TODO: rozhodnout jak se to tu udela
}

Dataset::~Dataset() {
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
Sequence* Dataset::newSequence() {
    return (new Sequence(*this));
}