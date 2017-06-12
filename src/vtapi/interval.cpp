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
#include <utility>

using namespace std;

namespace vtapi {


//================================= INTERVAL ===================================

Interval::Interval(const Interval &copy)
    : Interval(dynamic_cast<const Commons&>(copy), _context.selection)
{}

Interval::Interval(const Commons& commons, const string& selection)
    : KeyValues(commons, selection)
{
    if (_context.dataset.empty())
        throw BadConfigurationException("dataset not specified");

    _select.setOrderBy(def_col_int_id);

    if (!_context.sequence.empty())
        _select.querybuilder().whereString(def_col_int_seqname, _context.sequence);
    if (!_context.task.empty())
        _select.querybuilder().whereString(def_col_int_taskname, _context.task);
}

Interval::~Interval()
{}

bool Interval::next()
{
    bool ret = KeyValues::next();

    // destroy parent sequence if it changed
    if (ret) {
        if (_pparent_vid && _pparent_vid->getName() != this->getParentSequenceName())
            _pparent_vid.reset();
    }

    return ret;
}

Dataset *Interval::getParentDataset() const
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

Task *Interval::getParentTask() const
{
    string taskname;
    if (!_context.task.empty())
        taskname = _context.task;
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

string Interval::getParentSequenceName() const
{
    string seqname;
    if (!_context.sequence.empty())
        return _context.sequence;
    else
        return this->getString(def_col_int_seqname);
}

Sequence *Interval::getParentSequence() const
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

int Interval::getId() const
{
    return this->getInt(def_col_int_id);
}

unsigned int Interval::getStartTime() const
{
    return this->getInt(def_col_int_t1);
}

unsigned int Interval::getEndTime() const
{
    return this->getInt(def_col_int_t2);
}

chrono::system_clock::time_point Interval::calculateRealStartTime()
{
    if (!_pparent_vid) {
        _pparent_vid = std::make_shared<Video>(*this, this->getParentSequenceName());
        if (!_pparent_vid) return chrono::system_clock::time_point();
    }

    chrono::system_clock::time_point start = _pparent_vid->getRealStartTime();
    double fps = _pparent_vid->getFPS();
    double speed = _pparent_vid->getSpeed();

    if (start.time_since_epoch() > chrono::seconds::zero() && fps > 0) {
        double sec = this->getStartTime() * speed / fps;
        return start + chrono::microseconds(static_cast<chrono::microseconds::rep>(sec * 1000 * 1000));
    }
    else {
        return chrono::system_clock::time_point();
    }
}

chrono::system_clock::time_point Interval::calculateRealEndTime()
{
    if (!_pparent_vid) {
        _pparent_vid = std::make_shared<Video>(*this, this->getParentSequenceName());
        if (!_pparent_vid) return chrono::system_clock::time_point();
    }

    chrono::system_clock::time_point start = _pparent_vid->getRealStartTime();
    double fps = _pparent_vid->getFPS();
    double speed = _pparent_vid->getSpeed();

    if (start.time_since_epoch() > chrono::seconds::zero() && fps > 0) {
        double sec = (this->getEndTime() + 1) * speed / fps;
        return start + chrono::microseconds(static_cast<chrono::microseconds::rep>(sec * 1000 * 1000));
    }
    else {
        return chrono::system_clock::time_point();
    }
}

double Interval::getLengthSeconds() const
{
    return this->getFloat8(def_col_int_seclength);
}

bool Interval::preUpdate()
{
    return update().querybuilder().whereInt(def_col_int_id, this->getId());
}

bool Interval::updateStartTime(unsigned int t1)
{
    return this->updateInt(def_col_int_t1, t1);
}

bool Interval::updateEndTime(unsigned int t2)
{
    return this->updateInt(def_col_int_t2, t2);
}

bool Interval::filterById(const int id)
{
    return _select.querybuilder().whereInt(def_col_int_id, id);
}
bool Interval::filterBySequence(const string& seqname)
{
    return _select.querybuilder().whereString(def_col_int_seqname, seqname);
}

bool Interval::filterBySequences(const vector<string> &seqnames)
{
    return _select.querybuilder().whereStringVector(def_col_int_seqname, seqnames);
}

bool Interval::filterByTask(const string& taskname)
{
    return _select.querybuilder().whereString(def_col_int_taskname, taskname);
}

bool Interval::filterByEvent(const string& eventkey, const string& taskname,
                             const vector<string>& seqnames, const EventFilter & filter)
{
    return _select.querybuilder().whereEvent(eventkey, taskname, seqnames, filter);
}


//=================================== IMAGE ====================================

Image::Image(const Commons& commons,
             const string& selection,
             const string& name)
    : Interval(commons, selection)
{
    if (!name.empty())
        _select.querybuilder().whereString(def_col_int_imglocation, name);
}

bool Image::next()
{
    return Interval::next();
}

string Image::getDataLocation()
{
    if (_context.dataset_location.empty()) {
        Dataset *d = getParentDataset();
        _context.dataset_location = d->getLocation();
        delete d;
    }

    if (_context.sequence_location.empty()) {
        Sequence *s = getParentSequence();
        _context.sequence_location = s->getLocation();
        delete s;
    }

    return config().datasets_dir + Poco::Path::separator() +
            _context.dataset_location + Poco::Path::separator() +
            _context.sequence_location  + Poco::Path::separator() +
            this->getString(def_col_int_imglocation);
}

int Image::getTime()
{
    return this->getStartTime();
}

cv::Mat Image::getImageData()
{
    cv::Mat image = cv::imread(this->getDataLocation().c_str(), CV_LOAD_IMAGE_COLOR);
    if (!image.data)
        throw RuntimeException("Failed to open image: " + this->getDataLocation());

    return image;
}


}
