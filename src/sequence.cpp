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

using namespace std;

namespace vtapi {


//================================ SEQUENCE ====================================


Sequence::Sequence(const KeyValues& orig, const string& name) : KeyValues(orig)
{
    thisClass = "Sequence";

    if (!name.empty()) sequence = name;

    select = new Select(orig);
    select->from("sequences", "*");
    if (!sequence.empty()) select->whereString("seqname", sequence);
}

bool Sequence::next()
{
    if (KeyValues::next()) {
        this->sequence = this->getName();
        this->sequenceLocation = this->getLocation();
        return true;
    }
    else {
        return false;
    }
}

string Sequence::getName()
{
    return this->getString("seqname");
}
string Sequence::getLocation()
{
    return this->getString("seqlocation");
}
string Sequence::getType()
{
    return this->getString("seqtype");
}


bool Sequence::add(const string& name, const string& location, const string& type)
{
    return add(name, location, type, getUser(), std::string());
}

bool Sequence::add(const string& name, const string& location, const string& type,
    const string& userid, const string& notes)
{
    bool retval = true;

    retval &= KeyValues::preAdd(this->getDataset() + ".sequences");
    retval &= insert->keyString("seqname", name);
    retval &= insert->keyString("seqlocation", location);
    retval &= insert->keySeqtype("seqtyp", type);
    if (!userid.empty()) retval &= insert->keyString("userid", userid);
    if (!notes.empty()) retval &= insert->keyString("notes", notes);

    return retval;
}

bool Sequence::preUpdate()
{
    bool ret = KeyValues::preUpdate("sequences");
    if (ret) {
        ret &= update->whereString("seqname", this->sequence);
    }

    return ret;
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



Video::Video(const KeyValues& orig, const string& name) : Sequence(orig, name)
{
    thisClass = "Video";

    select->whereSeqtype("seqtyp", "video");
}

Video::~Video()
{
    closeVideo();
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
        
	this->closeVideo();
    
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

bool Video::next()
{
#if HAVE_OPENCV
    closeVideo();
#endif
    
    return Sequence::next();
}

#if HAVE_OPENCV

bool Video::openVideo()
{
    closeVideo();
    this->capture = cv::VideoCapture(this->getDataLocation());
    return this->capture.isOpened();
}

void Video::closeVideo()
{
    if (this->capture.isOpened()) this->capture.release();
}

cv::Mat Video::getData()
{
    cv::Mat frame;

    if (this->capture.isOpened() || this->openVideo()) {
        this->capture >> frame;
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
