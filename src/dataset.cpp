/**
 * @file    dataset.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Dataset class
 */

#include <common/vtapi_global.h>
#include <data/vtapi_sequence.h>
#include <data/vtapi_interval.h>
#include <data/vtapi_method.h>
#include <data/vtapi_process.h>
#include <data/vtapi_dataset.h>


using std::string;

using namespace vtapi;


Dataset::Dataset(const KeyValues& orig, const string& name) : KeyValues(orig) {
    thisClass = "Dataset";

    // set the dataset name
    if (!name.empty()) {
        dataset = name;
    }
    if (dataset.empty()) {
        logger->warning(313, "No dataset specified", thisClass+"::Dataset()");
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

string Dataset::getName() {
    return this->getString("dsname");
}


string Dataset::getLocation() {
    return this->getString("dslocation");
}


Sequence* Dataset::newSequence(const string& name) {
    return (new Sequence(*this, name));
}

Video* Dataset::newVideo(const string& name) {
    return (new Video(*this, name));
}

Method* Dataset::newMethod(const string& name) {
    return (new Method(*this, name));
}

Process* Dataset::newProcess(const string& name) {
    return (new Process(*this, name));
}