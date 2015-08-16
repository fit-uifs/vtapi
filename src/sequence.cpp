/**
 * @file
 * @brief   Methods of Sequence, Video and VideoPlayer classes
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
#include <vtapi/common/defs.h>
#include <vtapi/data/sequence.h>

using namespace std;

namespace vtapi {


//================================ SEQUENCE ====================================


Sequence::Sequence(const Commons& commons, const string& name)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        throw exception();
    
    if (!name.empty())
        context().sequence = name;

    _select.from(def_tab_sequences, def_col_all);
    _select.orderBy(def_col_seq_name);
    
    if (!context().sequence.empty())
        _select.whereString(def_col_seq_name, context().sequence);
}

Sequence::Sequence(const Commons& commons, const list<string>& names)
    : KeyValues(commons)
{
    if (context().dataset.empty())
        VTLOG_WARNING("Dataset is not specified");
    
    _select.from(def_tab_sequences, def_col_all);

    _select.whereStringInList(def_col_seq_name, names);
}

Sequence::~Sequence()
{}

bool Sequence::next()
{
    if (KeyValues::next()) {
        context().sequence = this->getName();
        context().sequenceLocation = this->getLocation();
        return true;
    }
    else {
        return false;
    }
}

Dataset *Sequence::getParentDataset()
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

string Sequence::getName()
{
    return this->getString(def_col_seq_name);
}

string Sequence::getType()
{
    return this->getString(def_col_seq_type);
}

string Sequence::getLocation()
{
    return this->getString(def_col_seq_location);
}

string Sequence::getComment()
{
    return this->getString(def_col_seq_comment);
}

std::string Sequence::getDataLocation()
{
    if (context().datasetLocation.empty()) {
        Dataset *d = getParentDataset();
        context().datasetLocation = d->getLocation();
        delete d;
    }
    
    return config().datasets_dir + Poco::Path::separator() +
            context().datasetLocation + Poco::Path::separator() +
            context().sequenceLocation;
}

time_t vtapi::Sequence::getCreatedTime()
{
    return this->getTimestamp(def_col_seq_created);
}

bool Sequence::updateComment(const std::string& comment)
{
    return this->updateString(def_col_seq_comment, comment);
}

bool Sequence::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_sequences);
    if (ret) {
        ret &= _update->whereString(def_col_seq_name, context().sequence);
    }

    return ret;
}

//============================== IMAGE FOLDER ===================================

ImageFolder::ImageFolder(const Commons& commons, const string& name)
    : Sequence(commons, name)
{
    _select.whereSeqtype(def_col_seq_type, def_val_images);
}

ImageFolder::ImageFolder(const Commons& commons, const list<string>& names)
    : Sequence(commons, names)
{
    _select.whereSeqtype(def_col_seq_type, def_val_images);
}

bool ImageFolder::next()
{
    return Sequence::next();
}

//================================= VIDEO ======================================

Video::Video(const Commons& commons, const string& name)
    : Sequence(commons, name)
{
    _select.whereSeqtype(def_col_seq_type, def_val_video);
}

Video::Video(const Commons& commons, const list<string>& names)
    : Sequence(commons, names)
{
    _select.whereSeqtype(def_col_seq_type, def_val_video);
}

Video::~Video()
{
    closeVideo();
}

bool Video::next()
{
    closeVideo();
    
    return Sequence::next();
}

bool Video::openVideo()
{
    closeVideo();
    _capture = cv::VideoCapture(this->getDataLocation());
    return _capture.isOpened();
}

void Video::closeVideo()
{
    if (_capture.isOpened()) _capture.release();
}

cv::VideoCapture& Video::getCapture()
{
    return _capture;
}

cv::Mat Video::getNextFrame()
{
    cv::Mat frame;

    if (_capture.isOpened() || this->openVideo()) {
        _capture >> frame;
    }
    
    return frame;
}

size_t Video::getLength()
{
    return getInt(def_col_seq_vidlength);
}

double Video::getFPS()
{
    return getFloat8(def_col_seq_vidfps);
}

double vtapi::Video::getSpeed()
{
    return getFloat8(def_col_seq_vidspeed);
}

time_t Video::getRealStartTime()
{
    return getTimestamp(def_col_seq_vidtime);
}

bool Video::updateRealStartTime(const time_t& starttime)
{
    return (updateTimestamp(def_col_seq_vidtime, starttime) && updateExecute());
}

}
