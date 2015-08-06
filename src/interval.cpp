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

#include <common/vtapi_global.h>
#include <data/vtapi_sequence.h>
#include <data/vtapi_interval.h>

using namespace std;

namespace vtapi {


//================================= INTERVAL ===================================

Interval::Interval(const Commons& commons, const string& selection)
    : KeyValues(commons)
{
    if (_context.dataset.empty())
        VTLOG_WARNING("Dataset is not specified");

    if (!selection.empty())
        _context.selection = selection;

    _select.from(_context.selection , def_col_all);
    _select.orderBy(def_col_int_id);
    if (!_context.sequence.empty())
        _select.whereString(def_col_int_seqname, _context.sequence);
    if (!_context.task.empty())
        _select.whereString(def_col_int_taskname, _context.task);
}

Interval::~Interval()
{}

bool Interval::next()
{
    return KeyValues::next();
}

int Interval::getId()
{
    return this->getInt(def_col_int_id);
}
string Interval::getTaskName()
{
    return this->getString(def_col_int_taskname);
}
string Interval::getSequenceName()
{
    return this->getString(def_col_int_seqname);
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
    Sequence seq(*this, this->getSequenceName());
    
    if (seq.next() && seq.getType().compare(def_val_video) == 0) {
        time_t start = seq.getTimestamp(def_col_seq_vidtime);
        float fps = seq.getFloat(def_col_seq_vidfps);

        if (start && fps) {
            *t1 = start + (time_t)(this->getStartTime() / fps);
            *t2 = start + (time_t) (this->getEndTime() / fps);
            bRet = true;
        }
    } 
    
    return bRet;
}

bool Interval::preUpdate()
{
    bool ret = KeyValues::preUpdate(_context.selection);
    if (ret) {
        ret &= _update->whereInt(def_col_int_id, this->getId());
    }

    return ret;
}
bool Interval::updateStartEndTime(const int t1, const int t2)
{
    return (updateInt(def_col_int_t1, t1) && updateInt(def_col_int_t2, t2) && updateExecute());
}


//bool Interval::add(const string& sequence, const int t1, const int t2, const string& location)
//{
//    return add(sequence, t1, t2, location, getUser(), "");
//}
//
//bool Interval::add(const string& sequence, const int t1, const int t2, const string& location,
//    const string& userid, const string& notes)
//{
//    bool retval = true;
//    int te2 = (t2 < 0) ? t1 : t2;
//
//    retval &= KeyValues::preAdd(this->selection);
//    retval &= insert->keyString("seqname", sequence);
//    retval &= insert->keyString("prsname", this->process);
//    retval &= insert->keyInt("t1", t1);
//    retval &= insert->keyInt("t2", te2);
//    retval &= insert->keyString("imglocation", location);
//    if (!userid.empty()) retval &= insert->keyString("userid", userid);
//    if (!notes.empty()) retval &= insert->keyString("notes", notes);
//    
//    return retval;
//}


bool Interval::filterById(const int id)
{
    return _select.whereInt(def_col_int_id, id);
}
bool Interval::filterBySequence(const string& seqname)
{
    return _select.whereString(def_col_int_seqname, seqname);
}
bool Interval::filterByTask(const string& taskname)
{
    return _select.whereString(def_col_int_taskname, taskname);
}

bool Interval::filterByDuration(const float t_low, const float t_high)
{
    return
        _select.whereFloat(def_col_int_seclength, t_low, ">=") &&
        _select.whereFloat(def_col_int_seclength, t_high, "<=");
}

bool Interval::filterByTimeRange(const time_t t_low, const time_t t_high)
{
    return _select.whereTimeRange(def_col_int_rtstart, def_col_int_seclength, t_low, t_high - t_low, "&&");
}

bool Interval::filterByRegion(const IntervalEvent::box& region)
{
    //TODO: wtf
    return _select.whereRegion("event,region", region, "&&");
}

//=================================== IMAGE ====================================

Image::Image(const Commons& commons, const string& name, const string& selection)
    : Interval(commons, selection)
{
    if (!name.empty())
        _select.whereString(def_col_int_imglocation, name);
    
}

bool Image::next()
{
    if (_image.data) _image.release();
    
    return Interval::next();
}

string Image::getDataLocation()
{
    return _config->baseLocation + _context.datasetLocation +
        _context.sequenceLocation  + this->getString(def_col_int_imglocation);
}

int Image::getTime()
{
    return this->getStartTime();
}

#if VTAPI_HAVE_OPENCV

cv::Mat& Image::getImageData()
{
    if (_image.data) _image.release();

    _image = cv::imread(this->getDataLocation().c_str(), CV_LOAD_IMAGE_COLOR);
    return _image;
}
#endif

////bool Image::add(const string& sequence, const int t, const string& location)
////{
////    return ((Interval*)this)->add(sequence, t, t, location);
////}

}
