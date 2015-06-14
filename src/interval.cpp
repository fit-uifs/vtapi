/**
 * @file
 * @brief   Methods of Interval and Image classes
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

using namespace std;

namespace vtapi {


//================================= INTERVAL ===================================

Interval::Interval(const KeyValues& orig, const string& selection) : KeyValues(orig)
{
    thisClass = "Interval";

    if (!selection.empty()) this->selection = selection;

    this->select = new Select(orig);
    this->select->from(this->selection, "*");
    if (!sequence.empty()) select->whereString("seqname", this->sequence);
    if (!process.empty()) select->whereString("prsname", this->process);
}

Interval::~Interval()
{}

bool Interval::next()
{
    return KeyValues::next();
}

int Interval::getId()
{
    return this->getInt("id");
}
string Interval::getProcessName()
{
    return this->getString("prsname");
}
string Interval::getSequenceName()
{
    return this->getString("seqname");
}
int Interval::getStartTime()
{
    return this->getInt("t1");
}
int Interval::getEndTime()
{
    return this->getInt("t2");
}
bool Interval::getRealStartEndTime(time_t *t1, time_t *t2)
{
    bool bRet = false;
    Sequence *seq = new Sequence(*this, this->getSequenceName());
    
    if(seq) {
        if (seq->next() && seq->getType().compare("video") == 0) {
            time_t start = seq->getTimestamp("vid_time");
            float fps = seq->getFloat("vid_fps");
            
            if (start && fps) {
                *t1 = start + (time_t)(getStartTime() / fps);
                *t2 = start + (time_t) (getEndTime() / fps);
                bRet = true;
            }
        } 
        delete seq;
    }
    
    return bRet;
}

bool Interval::preUpdate()
{
    bool ret = KeyValues::preUpdate(this->selection);
    if (ret) {
        ret &= update->whereString("seqname", this->sequence);
        ret &= update->whereString("prsname", this->process);
        ret &= update->whereInt("t1", this->getInt("t1"));
        ret &= update->whereInt("t2", this->getInt("t2"));
    }

    return ret;
}
bool Interval::updateStartEndTime(const int t1, const int t2)
{
    return (updateInt("t1", t1) && updateInt("t2", t2) && updateExecute());
}


bool Interval::add(const string& sequence, const int t1, const int t2, const string& location)
{
    return add(sequence, t1, t2, location, getUser(), "");
}

bool Interval::add(const string& sequence, const int t1, const int t2, const string& location,
    const string& userid, const string& notes)
{
    bool retval = true;
    int te2 = (t2 < 0) ? t1 : t2;

    retval &= KeyValues::preAdd(this->selection);
    retval &= insert->keyString("seqname", sequence);
    retval &= insert->keyString("prsname", this->process);
    retval &= insert->keyInt("t1", t1);
    retval &= insert->keyInt("t2", te2);
    retval &= insert->keyString("imglocation", location);
    if (!userid.empty()) retval &= insert->keyString("userid", userid);
    if (!notes.empty()) retval &= insert->keyString("notes", notes);
    
    return retval;
}


bool Interval::filterById(const int id)
{
    return select->whereInt("id", id);
}
bool Interval::filterBySequence(const string& seqname)
{
    return select->whereString("seqname", seqname);
}
bool Interval::filterByProcess(const string& prsname)
{
    return select->whereString("prsname", prsname);
}

bool Interval::filterByDuration(const float t_low, const float t_high)
{
    return
        select->whereFloat("sec_length", t_low, ">=") &&
        select->whereFloat("sec_length", t_high, "<=");
}

bool Interval::filterByTimeRange(const time_t t_low, const time_t t_high)
{
    return select->whereTimeRange("rt_start", "sec_length", t_low, t_high - t_low, "&&");
}

bool Interval::filterByRegion(const IntervalEvent::box& region)
{
    return select->whereRegion("event,region", region, "&&");
}

//=================================== IMAGE ====================================


Image::Image(const KeyValues& orig, const std::string& name, const string& selection) : Interval(orig, selection)
{
    thisClass = "Image";

    if (!name.empty()) select->whereString("imglocation", name);
}

string Image::getImgLocation()
{
    return this->getString("imglocation");
}

string Image::getDataLocation()
{
    return (this->getDataLocation() + this->getImgLocation());
}

int Image::getTime()
{
    return this->getStartTime();
}

#if HAVE_OPENCV

cv::Mat Image::getData()
{
    if (this->image.data) this->image.release();

    this->image = cv::imread(this->getDataLocation().c_str(), CV_LOAD_IMAGE_COLOR);
    return this->image;
}
#endif

bool Image::add(const string& sequence, const int t, const string& location)
{
    return ((Interval*)this)->add(sequence, t, t, location);
}

}
