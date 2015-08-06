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

#include <common/vtapi_global.h>
#include <data/vtapi_sequence.h>

using namespace std;

namespace vtapi {


//================================ SEQUENCE ====================================


Sequence::Sequence(const Commons& commons, const string& name)
    : KeyValues(commons)
{
    if (_context.dataset.empty())
        VTLOG_WARNING("Dataset is not specified");
    
    if (!name.empty())
        _context.sequence = name;

    _select.from(def_tab_sequences, def_col_all);
    
    if (!_context.sequence.empty())
        _select.whereString(def_col_seq_name, _context.sequence);
}

Sequence::Sequence(const Commons& commons, const list<string>& names)
    : KeyValues(commons)
{
    if (_context.dataset.empty())
        VTLOG_WARNING("Dataset is not specified");
    
    _select.from(def_tab_sequences, def_col_all);

    _select.whereStringInList(def_col_seq_name, names);
}

Sequence::~Sequence()
{}

bool Sequence::next()
{
    if (KeyValues::next()) {
        _context.sequence = this->getName();
        _context.sequenceLocation = this->getString(def_col_seq_location);
        return true;
    }
    else {
        return false;
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

string Sequence::getComment()
{
    return this->getString(def_col_seq_comment);
}

std::string Sequence::getDataLocation()
{
    if (_context.sequenceLocation.empty())
        VTLOG_WARNING("Sequence location is unknown");
    
    return _config->baseLocation + _context.datasetLocation + _context.sequenceLocation;
}


bool Sequence::updateComment(const std::string& comment)
{
    return this->updateString(def_col_seq_comment, comment);
}

bool Sequence::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_sequences);
    if (ret) {
        ret &= _update->whereString(def_col_seq_name, _context.sequence);
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
#if VTAPI_HAVE_OPENCV
    closeVideo();
#endif
    
    return Sequence::next();
}

#if VTAPI_HAVE_OPENCV

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
    return getInt("vid_length");
}

float Video::getFPS()
{
    return getFloat("vid_fps");
}

time_t Video::getRealStartTime()
{
    return getTimestamp("vid_time");
}

bool Video::updateRealStartTime(const time_t& starttime)
{
    return (updateTimestamp("vid_time", starttime) && updateExecute());
}

#endif

}
