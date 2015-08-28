/**
 * @file
 * @brief   Methods of Interval and Image classes
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <Poco/Path.h>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/data/sequence.h>
#include <vtapi/data/interval.h>

using namespace std;

namespace vtapi {


//================================= INTERVAL ===================================

Interval::Interval(const Commons& commons, const string& selection)
    : KeyValues(commons, selection)
{
    if (context().dataset.empty())
        throw BadConfigurationException("dataset not specified");

    select().setOrderBy(def_col_int_id);

    if (!context().sequence.empty())
        select().whereString(def_col_int_seqname, context().sequence);
    if (!context().task.empty())
        select().whereString(def_col_int_taskname, context().task);
}

Interval::~Interval()
{}

bool Interval::next()
{
    return KeyValues::next();
}

Dataset *Interval::getParentDataset()
{
    Dataset *d = new Dataset(*this);
    if (d->next()) {
        return d;
    }
    else {
        delete d;
        return NULL;
    }
}

Task *Interval::getParentTask()
{
    string taskname;
    if (!context().task.empty())
        taskname = context().task;
    else
        taskname = this->getString(def_col_int_taskname);

    if (!taskname.empty()) {
        Task *t = new Task(*this, taskname);
        if (t->next()) {
            return t;
        }
        else {
            delete t;
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

string Interval::getParentSequenceName()
{
    string seqname;
    if (!context().sequence.empty())
        return context().sequence;
    else
        return this->getString(def_col_int_seqname);
}

Sequence *Interval::getParentSequence()
{
    string seqname = getParentSequenceName();

    if (!seqname.empty()) {
        Sequence *s = new Sequence(*this, seqname);
        if (s->next()) {
            return s;
        }
        else {
            delete s;
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

int Interval::getId()
{
    return this->getInt(def_col_int_id);
}

int Interval::getStartTime()
{
    return this->getInt(def_col_int_t1);
}

int Interval::getEndTime()
{
    return this->getInt(def_col_int_t2);
}

bool Interval::getRealStartEndTime(time_t *t1, time_t *t2)
{
    bool bRet = false;
    Video *vid = new Video(*this, getParentSequenceName());

    if (vid->next()) {
        time_t start = vid->getRealStartTime();
        double fps = vid->getFPS();

        if (start && fps) {
            *t1 = start + (time_t)(this->getStartTime() / fps);
            *t2 = start + (time_t) (this->getEndTime() / fps);
            bRet = true;
        }
    }
    delete vid;
    
    return bRet;
}

double Interval::getLengthSeconds()
{
    return this->getFloat8(def_col_int_seclength);
}

bool Interval::preUpdate()
{
    return update().whereInt(def_col_int_id, this->getId());
}
bool Interval::updateStartEndTime(const int t1, const int t2)
{
    return (updateInt(def_col_int_t1, t1) && updateInt(def_col_int_t2, t2) && updateExecute());
}

bool Interval::filterById(const int id)
{
    return select().whereInt(def_col_int_id, id);
}
bool Interval::filterBySequence(const string& seqname)
{
    return select().whereString(def_col_int_seqname, seqname);
}

bool Interval::filterBySequences(const std::list<string> &seqnames)
{
    return select().whereStringInList(def_col_int_seqname, seqnames);
}

bool Interval::filterByTask(const string& taskname)
{
    return select().whereString(def_col_int_taskname, taskname);
}

bool Interval::filterByDuration(const float t_low, const float t_high)
{
    return
        select().whereFloat(def_col_int_seclength, t_low, ">=") &&
        select().whereFloat(def_col_int_seclength, t_high, "<=");
}

bool Interval::filterByTimeRange(const time_t t_low, const time_t t_high)
{
    return select().whereTimeRange(def_col_int_rtstart, def_col_int_seclength, t_low, t_high - t_low, "&&");
}

bool Interval::filterByRegion(const IntervalEvent::box& region)
{
    return select().whereRegion("event,region", region, "&&");
}

//=================================== IMAGE ====================================

Image::Image(const Commons& commons,
             const string& selection,
             const string& name)
    : Interval(commons, selection)
{
    if (!name.empty())
        select().whereString(def_col_int_imglocation, name);
}

bool Image::next()
{
    if (_image.data) _image.release();
    
    return Interval::next();
}

string Image::getDataLocation()
{
    if (context().datasetLocation.empty()) {
        Dataset *d = getParentDataset();
        context().datasetLocation = d->getLocation();
        delete d;
    }

    if (context().sequenceLocation.empty()) {
        Sequence *s = getParentSequence();
        context().sequenceLocation = s->getLocation();
        delete s;
    }

    return config().datasets_dir + Poco::Path::separator() +
            context().datasetLocation + Poco::Path::separator() +
            context().sequenceLocation  + Poco::Path::separator() +
            this->getString(def_col_int_imglocation);
}

int Image::getTime()
{
    return this->getStartTime();
}

cv::Mat& Image::getImageData()
{
    if (_image.data) _image.release();

    _image = cv::imread(this->getDataLocation().c_str(), CV_LOAD_IMAGE_COLOR);
    return _image;
}


}
