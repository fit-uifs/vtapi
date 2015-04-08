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

using std::string;

using namespace vtapi;


//================================= INTERVAL ===================================

Interval::Interval(const KeyValues& orig, const string& selection) : KeyValues(orig) {
    thisClass = "Interval";

    if (!selection.empty()) this->selection = selection;

    this->select = new Select(orig);
    this->select->from(this->selection, "*");
    if (!this->sequence.empty()) {
        this->select->whereString("seqname", this->sequence);
    }
    if (!this->process.empty()) {
        this->select->whereString("prsname", this->process);
    }
}

int Interval::getId() {
    return this->getInt("id");
}

string Interval::getSequenceName() {
    return this->getString("seqname");
}

string Interval::getProcessName() {
    return this->getString("prsname");
}

Sequence* Interval::getParentSequence() {
    return new Sequence(*this, this->getSequenceName());
}



int Interval::getStartTime() {
    return this->getInt("t1");
}

int Interval::getEndTime() {
    return this->getInt("t2");
}

bool Interval::getRealStartEndTime(time_t *t1, time_t *t2)
{
    bool bRet = false;
    Sequence *seq = this->getParentSequence();
    
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


void Interval::setStartEndTime(const int t1, const int t2)
{
    preSet();
    update->setInt("t1", t1);
    update->setInt("t2", t2);
    update->execute();
}



bool Interval::add(const string& sequence, const int t1, const int t2, const string& location)
{
    return add(sequence, t1, t2, location, getUser(), "");
}

bool Interval::add(const string& sequence, const int t1, const int t2, const string& location,
    const string& userid, const string& notes)
{
    bool retval = VT_OK;
    int te2 = (t2 < 0) ? t1 : t2;

    if (insert) store.push_back(insert);
    insert = new Insert(*this, this->selection);
    retval &= insert->keyString("seqname", sequence);
    retval &= insert->keyString("prsname", this->process);
    retval &= insert->keyInt("t1", t1);
    retval &= insert->keyInt("t2", te2);
    retval &= insert->keyString("imglocation", location);
    if (!userid.empty()) retval &= insert->keyString("userid", userid);
    if (!notes.empty()) retval &= insert->keyString("notes", notes);
    
    return retval;
}

bool Interval::preSet() {
    bool retval = VT_OK;

    vt_destruct(update);
    update = new Update(*this, this->selection);
    retval &= update->whereString("seqname", this->sequence);
    retval &= update->whereString("prsname", this->process);
    retval &= update->whereInt("t1", this->getInt("t1"));
    retval &= update->whereInt("t2", this->getInt("t2"));

    return retval;
}

void Interval::filterBySequence(const string& seqname) {
    select->whereString("seqname", seqname);
}

//=================================== IMAGE ====================================


Image::Image(const KeyValues& orig, const string& selection) : Interval(orig, selection) {
    thisClass = "Image";
}

int Image::getTime() {
    int t1 = this->getStartTime();
    int t2 = this->getEndTime();

    if (t1 != t2) {
        logger->warning(3291, "This is not an Image (see output if verbose)", thisClass+"::getTime()");
        // if (verbose) this->print();
    }
    return t1;
}

bool Image::add(const string& sequence, const int t, const string& location) {
    return ((Interval*)this)->add(sequence, t, t, location);
}

string Image::getImgLocation() {
    return this->getString("imglocation");
}

string Image::getDataLocation() {
    return (this->getDataLocation() + this->getImgLocation());
}

#if HAVE_OPENCV
cv::Mat Image::getData() {
    if (this->image.data) this->image.release();
    
    this->image = cv::imread(this->getDataLocation().c_str(), CV_LOAD_IMAGE_COLOR);
    return this->image;    
}
#endif