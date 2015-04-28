/**
 * @file
 * @brief   Methods of Dataset class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
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
    if (!dataset.empty()) select->whereString("dsname", dataset);
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

ImageFolder* Dataset::newImageFolder(const string& name)
{
    return (new ImageFolder(*this, name));
}


Method* Dataset::newMethod(const string& name) {
    return (new Method(*this, name));
}

Process* Dataset::newProcess(const string& name) {
    return (new Process(*this, name));
}