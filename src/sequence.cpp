/**
 * @file
 * @brief   Methods of Sequence, Video and VideoPlayer classes
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
using std::vector;

using namespace vtapi;


//================================ SEQUENCE ====================================


Sequence::Sequence(const KeyValues& orig, const string& name) : KeyValues(orig)
{
    thisClass = "Sequence";

    if (!name.empty()) this->sequence = name;

    select = new Select(orig);
    select->from("sequences", "*");
    if (!this->sequence.empty()) select->whereString("seqname", this->sequence);
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

string Sequence::getType() {
    return this->getString("seqtype");
}

Interval* Sequence::newInterval(const int t1, const int t2)
{
    return new Interval(*this);
}

Image* Sequence::newImage(const string& name)
{
    Image* image = new Image(*this);
    
    if (!name.empty()) image->select->whereString("imglocation", name);

    return image;
}


#if HAVE_OPENCV
cv::Mat Sequence::getData() {
    if (this->frame.data) this->frame.release();
    
    if (this->getType().compare("video")) {
        return dynamic_cast<Video*>(this)->getData();
    }
    else if (this->getType().compare("images")) {
        if (this->imageBuffer == NULL) {
            this->imageBuffer = this->newImage();
        }
        if (this->imageBuffer->next()) {
            this->frame = this->imageBuffer->getData();
            return frame;
        }
    }
    
    return frame;
}
#endif

bool Sequence::add(const string& name, const string& location, const string& type)
{
    return add(name, location, type, getUser(), "");
}

bool Sequence::add(const string& name, const string& location, const string& type,
    const string& userid, const string& notes)
{
    bool retval = VT_OK;

    if (insert) store.push_back(insert);
    insert = new Insert(*this, "sequences");
    retval &= insert->keyString("seqname", name);
    retval &= insert->keyString("seqlocation", location);
    retval &= insert->keySeqtype("seqtyp", type);
    if (!userid.empty()) retval &= insert->keyString("userid", userid);
    if (!notes.empty()) retval &= insert->keyString("notes", notes);

    return retval;
}

bool Sequence::preSet() {
    vt_destruct(update);

    update = new Update(*this, "sequences");
    update->whereString("seqname", this->sequence);

    return VT_OK;
}

//============================== IMAGE FOLDER ===================================

ImageFolder::ImageFolder(const KeyValues& orig, const string& name) : Sequence(orig, name)
{
    thisClass = "ImageFolder";

    select->whereSeqtype("seqtyp", "images");
}

bool ImageFolder::add(const string& name, const string& location)
{
    bool bRet = true;

    do {
        string fullpath = baseLocation + datasetLocation + location;

        if (!dirExists(fullpath)) {
            logger->warning(3210, "Cannot open folder " + fullpath, thisClass + "::add()");
            bRet = false;
            break;
        }

        this->sequence = name;
        this->sequenceLocation = location;

        bRet = Sequence::add(name, location, "images");

    } while(0);

    return bRet;
}
//================================= VIDEO ======================================



Video::Video(const KeyValues& orig, const string& name) : Sequence(orig, name) {
    thisClass = "Video";

    select->whereSeqtype("seqtyp", "video");
}

bool Video::add(const string& name, const string& location, const time_t& realtime)
{
    bool bRet = true;

    do {
#if HAVE_OPENCV
        string fullpath = baseLocation + datasetLocation + location;
        
        if (!fileExists(fullpath)) {
            logger->warning(3210, "Cannot open file " + fullpath, thisClass + "::add()");
            bRet = false;
            break;
        }

        this->sequence = name;
        this->sequenceLocation = location;

        bRet = openVideo();
        if (!bRet) {
            logger->warning(3210, "Cannot open video " + location, thisClass + "::add()");
            bRet = false;
            break;
        }
        
        size_t cnt_frames = (size_t)this->capture.get(CV_CAP_PROP_FRAME_COUNT);
        double fps = this->capture.get(CV_CAP_PROP_FPS);
        if ((cnt_frames == 0) || (fps == 0.0)) {
            logger->warning(3211, "Cannot get length and FPS of " + location, thisClass + "::add()");
            bRet = false;
            break;
        }

#endif
        bRet = Sequence::add(name, location, "video");
#if HAVE_OPENCV
        bRet &= addInt("vid_length", cnt_frames);
        bRet &= addFloat("vid_fps", fps);
        if (realtime > 0) bRet &= addTimestamp("vid_time", realtime);
#endif
    } while(0);

    return bRet;
}

#if HAVE_OPENCV

bool Video::openVideo() {
    closeVideo();
    this->capture = cv::VideoCapture(this->getDataLocation());
    return this->capture.isOpened();
}

void Video::closeVideo() {
    if (this->capture.isOpened()) this->capture.release();
}

cv::Mat Video::getData() {
    if (this->frame.data) this->frame.release();
    
    if (this->capture.isOpened() || this->openVideo()) {
        this->capture >> this->frame;
    }
    return this->frame;
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

bool Video::setRealStartTime(const time_t& starttime)
{
    return (setTimestamp("vid_time", starttime) && setExecute());
}

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