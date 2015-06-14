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
#include <data/vtapi_dataset.h>

using namespace std;

namespace vtapi {


Dataset::Dataset(const KeyValues& orig, const string& name) : KeyValues(orig)
{
    thisClass = "Dataset";

    // set the dataset name
    if (!name.empty()) {
        this->dataset = name;
    }
    if (this->dataset.empty()) {
        logger->warning(313, "No dataset specified", thisClass+"::Dataset()");
    }
    
    select = new Select(orig);
    select->from("public.datasets", "*");
    if (!this->dataset.empty()) select->whereString("dsname", this->dataset);
}

bool Dataset::next()
{
    if (KeyValues::next()) {
        this->dataset = this->getName();
        this->datasetLocation = this->getLocation();
        return true;
    }
    else {
        return false;
    }
}

string Dataset::getName()
{
    return this->getString("dsname");
}

string Dataset::getLocation()
{
    return this->getString("dslocation");
}

bool Dataset::preUpdate()
{
    bool ret = KeyValues::preUpdate("public.datasets");
    if (ret) {
        ret &= update->whereString("dsname", this->dataset);
    }
    
    return ret;
}

}
