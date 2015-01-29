/**
 * @file    interval.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Interval and Image classes
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

    select = new Select(orig);
    select->from(this->selection, "*");
    select->whereString("seqname", this->sequence);

}

string Interval::getSequenceName() {
    // TODO: possibly empty... possible warning 332
    return this->sequence;
}

//TODO: pres query
Sequence* Interval::getParentSequence() {
    Sequence *seq = NULL;
    if (!this->sequence.empty()) {
        seq = new Sequence(*this, this->sequence);
    }  
    return seq;
}



int Interval::getStartTime() {
    return this->getInt("t1");
}

int Interval::getEndTime() {
    return this->getInt("t2");
}


bool Interval::add(const string& sequence, const int t1, const int t2, const string& location) {
    bool retval = VT_OK;
    int te2 = (t2 < 0) ? t1 : t2;

    vt_destruct(insert);
    insert = new Insert(*this, this->selection);
    retval &= insert->keyString("seqname", sequence);
    retval &= insert->keyInt("t1", t1);
    retval &= insert->keyInt("t2", te2);
    retval &= insert->keyString("imglocation", location);

    // TODO: image && storage checking??

    return retval;
}

bool Interval::add(const string& sequence, const int t1, const int t2, const string& location,
    const string& userid, const string& notes) {
    bool retval = VT_OK;

    retval &= this->add(sequence, t1, t2, location);
    retval &= insert->keyString("userid", userid);
    retval &= insert->keyString("notes", notes);
    
    return retval;
}

bool Interval::addExecute() {
    bool retval = VT_OK;
    time_t now = 0;

    if (this->insert) {
        time(&now);
        retval &= insert->keyTimestamp("created", now);
        if (retval) retval &= insert->execute();
    }
    else retval = VT_FAIL;

    return retval;
}


bool Interval::preSet() {
    bool retval = VT_OK;

    vt_destruct(update);
    update = new Update(*this, this->selection);
    retval &= update->whereString("seqname", sequence);
    retval &= update->whereInt("t1", this->getInt("t1"));
    retval &= update->whereInt("t2", this->getInt("t2"));

    return retval;
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

#ifdef HAVE_OPENCV
cv::Mat Image::getData() {
    if (this->image.data) this->image.release();
    
    this->image = cv::imread(this->getDataLocation().c_str(), CV_LOAD_IMAGE_COLOR);
    return this->image;    
}
#endif