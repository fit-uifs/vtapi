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
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/data/sequence.h>

using namespace std;

namespace vtapi {


//================================ SEQUENCE ====================================


Sequence::Sequence(const Sequence &copy)
    : Sequence(dynamic_cast<const Commons&>(copy))
{
}

Sequence::Sequence(const Commons& commons, const string& name)
    : KeyValues(commons, def_tab_sequences)
{
    if (_context.dataset.empty() || _context.dataset_location.empty())
        throw BadConfigurationException("dataset not specified");

    if (!name.empty())
        _context.sequence = name;

    _select.setOrderBy(def_col_seq_name);

    if (!_context.sequence.empty())
        _select.querybuilder().whereString(def_col_seq_name, _context.sequence);
}

Sequence::Sequence(const Commons& commons, const vector<string>& names)
    : KeyValues(commons, def_tab_sequences)
{
    if (_context.dataset.empty() || _context.dataset_location.empty())
        throw BadConfigurationException("dataset not specified");

    _select.setOrderBy(def_col_seq_name);
    _select.querybuilder().whereStringVector(def_col_seq_name, names);
}

Sequence::~Sequence()
{}

bool Sequence::next()
{
    if (KeyValues::next()) {
        _context.sequence = this->getName();
        _context.sequence_location = this->getLocation();
        return true;
    }
    else {
        return false;
    }
}

Dataset *Sequence::getParentDataset() const
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

string Sequence::getName() const
{
    return this->getString(def_col_seq_name);
}

string Sequence::getType() const
{
    return this->getString(def_col_seq_type);
}

string Sequence::getLocation() const
{
    return this->getString(def_col_seq_location);
}

unsigned int Sequence::getLength() const
{
    return this->getInt(def_col_seq_length);
}

string Sequence::getComment() const
{
    return this->getString(def_col_seq_comment);
}

string Sequence::getDataLocation() const
{
    if (_context.sequence_location.empty())
        throw RuntimeException("sequence not ready, call next()?");

    return config().datasets_dir + Poco::Path::separator() +
            _context.dataset_location + Poco::Path::separator() +
            _context.sequence_location;
}

chrono::system_clock::time_point Sequence::getRealStartTime() const
{
    return getTimestamp(def_col_seq_vidtime);
}

bool Sequence::updateRealStartTime(const chrono::system_clock::time_point& starttime)
{
    return (updateTimestamp(def_col_seq_vidtime, starttime));
}

chrono::system_clock::time_point Sequence::getCreatedTime() const
{
    return this->getTimestamp(def_col_seq_created);
}

bool Sequence::updateComment(const string& comment)
{
    return this->updateString(def_col_seq_comment, comment);
}

bool Sequence::preUpdate()
{
    return update().querybuilder().whereString(def_col_seq_name, _context.sequence);
}

//============================== IMAGE FOLDER ===================================

ImageFolder::ImageFolder(const ImageFolder &copy)
    : ImageFolder(dynamic_cast<const Commons&>(copy))
{
}

ImageFolder::ImageFolder(const Commons& commons, const string& name)
    : Sequence(commons, name)
{
    _select.querybuilder().whereSeqtype(def_col_seq_type, def_val_images);
}

ImageFolder::ImageFolder(const Commons& commons, const vector<string>& names)
    : Sequence(commons, names)
{
    _select.querybuilder().whereSeqtype(def_col_seq_type, def_val_images);
}

bool ImageFolder::next()
{
    bool result;
    result = Sequence::next();
    this->imagesInPath.clear();

    Poco::DirectoryIterator it(this->getDataLocation());
    Poco::DirectoryIterator end;

    while (it != end) {
        this->imagesInPath.push_back(it->path());
        ++it;
    }

    std::sort(this->imagesInPath.begin(), this->imagesInPath.end());

    return result;
}

//================================= VIDEO ======================================

Video::Video(const Video &copy)
    : Video(dynamic_cast<const Commons&>(copy))
{
}

Video::Video(const Commons& commons, const string& name)
    : Sequence(commons, name)
{
    _select.querybuilder().whereSeqtype(def_col_seq_type, def_val_video);
}

Video::Video(const Commons& commons, const vector<string>& names)
    : Sequence(commons, names)
{
    _select.querybuilder().whereSeqtype(def_col_seq_type, def_val_video);
}

bool Video::next()
{
    return Sequence::next();
}

cv::VideoCapture Video::openVideo() const
{
    cv::VideoCapture cap(this->getDataLocation());
    if (!cap.isOpened())
        throw RuntimeException("Failed to open video: " + this->getDataLocation());

    return cap;
}

double Video::getFPS() const
{
    return getFloat8(def_col_seq_vidfps);
}

double vtapi::Video::getSpeed() const
{
    return getFloat8(def_col_seq_vidspeed);
}

}


bool operator >> (const vtapi::ImageFolder *imageFolder, cv::Mat &image) {
    try {
        image = cv::imread(imageFolder->imagesInPath.at(imageFolder->iNextImage), CV_LOAD_IMAGE_COLOR);

        if (! image.data) {
            throw vtapi::RuntimeException("Failed to open image \"" + imageFolder->imagesInPath.at(imageFolder->iNextImage) + "\" from imageFolder: " + imageFolder->getDataLocation());
        }

        imageFolder->iNextImage++;
    }
    catch (std::out_of_range e) {
        image.release();
    }

    return ! image.empty();
}