/**
 * @file    sequence.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Sequence, Video and VideoPlayer classes
 */

#include "data/vtapi_sequence.h"

using namespace vtapi;


//================================ SEQUENCE ====================================


Sequence::Sequence(const KeyValues& orig, const string& name) : KeyValues(orig) {
    thisClass = "Sequence";

    if (!name.empty()) this->sequence = name;

    select = new Select(orig);
    select->from("sequences", "*");
    select->whereString("seqname", this->sequence);
}

bool Sequence::next() {
    KeyValues* kv = ((KeyValues*)this)->next();
    if (kv) {
        this->sequence = this->getName();
        this->sequenceLocation = this->getLocation();
    }

    return kv;
}


string Sequence::getName() {
    return this->getString("seqname");
}

string Sequence::getLocation() {
    return this->getString("seqlocation");
}

Interval* Sequence::newInterval(const int t1, const int t2) {
    return new Interval(*this);
}

Image* Sequence::newImage(const string& name) {
    Image* image = new Image(*this);
    image->select->whereString("imglocation", name);

    return image;
}


bool Sequence::add(const string& name, const string& location, const string& type) {
    bool retval = VT_OK;

    destruct(insert);
    insert = new Insert(*this, "sequences");
    retval &= insert->keyString("seqname", name);
    retval &= insert->keyString("seqlocation", location);
    retval &= insert->keySeqtype("seqtyp", type);

    return retval;
}

bool Sequence::add(const string& name, const string& location, const string& type,
    const string& userid, const string& groupid, const string& notes) {
    bool retval = VT_OK;

    retval &= this->add(name, location, type);
    retval &= insert->keyString("userid", userid);
    retval &= insert->keyString("groupid", groupid);
    retval &= insert->keyString("notes", notes);

    return retval;
}

bool Sequence::addExecute() {
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

bool Sequence::preSet() {
    destruct(update);

    update = new Update(*this, "sequences");
    update->whereString("seqname", this->sequence);

    return VT_OK;
}

//================================= VIDEO ======================================



Video::Video(const KeyValues& orig, const string& name) : Sequence(orig, name) {
    thisClass = "Video";

    select->whereSeqtype("seqtyp", "video");
}

bool Video::add(string name, string location) {
    bool retval = VT_OK;
    string filename = "";

#ifdef __OPENCV_CORE_HPP__
    // TODO: P3k check something else???
    filename = baseLocation + datasetLocation + location;
    if (!fileExists(filename)) {
        logger->warning(3210, "Cannot open file " + filename, thisClass+"::add()");
        retval = VT_FAIL;
    }
    // TODO: check the lenght and so on...
#endif
    
    if (retval) {
        destruct(insert);
        insert = new Insert(*this, "sequences");
        retval &= insert->keyString("seqname", name);
        retval &= insert->keyString("seqlocation", location);
        retval &= insert->keySeqtype("seqtyp", "video");
    }
    return retval;
}

// TODO: nejak odlisit nestandardni veci


#ifdef __OPENCV_HIGHGUI_HPP__

VideoPlayer::VideoPlayer(Commons& orig) : Commons(orig) {
    thisClass = "VideoPlayer(Commons&)";
}

VideoPlayer::VideoPlayer(Video& orig) : Commons(orig) {
    thisClass = "VideoPlayer(Video&)";

    videos.push_back(orig);
}


// TODO: point (), line, box, ellipse
// TODO: play 1, 2, 4, 6, 9, 12 and 16 videos at once :)
// TODO: P3k dodelat nacteni vice Video a Interval
bool VideoPlayer::play() {
    bool retval = VT_OK;
    cv::VideoCapture cap;
    int fps = 25;

    if (!videos.empty()) {
        cap = cv::VideoCapture(videos.front().getDataLocation());
        int fps = (int) cap.get(CV_CAP_PROP_FPS);
    }
    // TODO: images, ...
    // frame = cv::imread("img.jpg");
    // if there is no file to play, use the default video capture device (camera)
    else {
        cap = cv::VideoCapture(0);
    }

    // toz a jedem
    if(cap.isOpened()) {
        cv::namedWindow("video",1);
        cv::Mat frame;
        while(1)
        {
            cap >> frame;
            cv::imshow("video", frame);
            if(cv::waitKey(1000 / fps) >= 0) break; // correct the with real timer!
        }
        cv::destroyWindow("video");
    }
    else {
        logger->warning(161, "Sorry, there is nothing to play, aborting :(", thisClass+"::play()");
        retval = VT_FAIL;
    }

    return retval;
}

#endif