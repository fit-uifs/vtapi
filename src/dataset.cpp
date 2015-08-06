/**
 * @file
 * @brief   Methods of Dataset class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <data/vtapi_dataset.h>

using namespace std;

namespace vtapi {


Dataset::Dataset(const Commons& commons, const string& name)
    : KeyValues(commons)
{
    // set the dataset name
    if (!name.empty())
        _context.dataset = name;
    
    _select.from(def_tab_datasets, def_col_all);
    
    if (!_context.dataset.empty())
        _select.whereString(def_col_ds_name, _context.dataset);
}

Dataset::Dataset(const Commons& commons, const list<string>& names)
    : KeyValues(commons)
{
    _select.from(def_tab_datasets, def_col_all);

    _select.whereStringInList(def_col_ds_name, names);
}

bool Dataset::next()
{
    if (KeyValues::next()) {
        _context.dataset = this->getName();
        _context.datasetLocation = this->getLocation();
        return true;
    }
    else {
        return false;
    }
}

string Dataset::getName()
{
    return this->getString(def_col_ds_name);
}

string Dataset::getFriendlyName()
{
    return this->getString(def_col_ds_fname);
}

string Dataset::getLocation()
{
    return this->getString(def_col_ds_location);
}

string Dataset::getDescription()
{
    return this->getString(def_col_ds_description);
}

bool Dataset::updateFriendlyName(const string& friendly_name)
{
    return this->updateString(def_col_ds_fname, friendly_name);
}

bool Dataset::updateDescription(const string& description)
{
    return this->updateString(def_col_ds_description, description);
}

Sequence* Dataset::createSequence(
    const string& name,
    const string& location,
    const string& type,
    const string& comment)
{
    Sequence *seq = NULL;

    bool retval = true;
    Insert insert(*this, def_tab_sequences);
    retval &= insert.keyString(def_col_seq_name, name);
    retval &= insert.keyString(def_col_seq_location, location);
    retval &= insert.keySeqtype(def_col_seq_type, type);
    if (!comment.empty()) retval &= insert.keyString(def_col_seq_comment, comment);
    
    if (retval && insert.execute()) {
        seq = loadSequences(name);
        if (!seq->next()) vt_destruct(seq);
    }

    return seq;
}

Video* Dataset::createVideo(
    const string& name,
    const string& location,
    const time_t& realtime,
    const string& comment)
{
    Video *vid = NULL;

    do {
#if VTAPI_HAVE_OPENCV
        string fullpath = _config->baseLocation + _context.datasetLocation + location;

        if (!fileExists(fullpath)) {
            VTLOG_WARNING( "File doesn't exist: " + fullpath);
            break;
        }

        cv::VideoCapture capture(fullpath);
        if (!capture.isOpened()) {
            VTLOG_WARNING( "Cannot open video: " + fullpath);
            break;
        }

        size_t cnt_frames = (size_t) capture.get(CV_CAP_PROP_FRAME_COUNT);
        double fps = capture.get(CV_CAP_PROP_FPS);
        if ((cnt_frames == 0) || (fps == 0.0)) {
            VTLOG_WARNING( "Cannot get length and FPS of " + fullpath);
            break;
        }

        capture.release();
#endif
        
        bool retval = true;
        Insert insert(*this, def_tab_sequences);
        retval &= insert.keyString(def_col_seq_name, name);
        retval &= insert.keyString(def_col_seq_location, location);
        retval &= insert.keySeqtype(def_col_seq_type, def_val_video);
        if (!comment.empty()) retval &= insert.keyString(def_col_seq_comment, comment);
#if VTAPI_HAVE_OPENCV
        retval &= insert.keyInt(def_col_seq_vidlength, cnt_frames);
        retval &= insert.keyFloat(def_col_seq_vidfps, fps);
        if (realtime > 0) retval &= insert.keyTimestamp(def_col_seq_vidtime, realtime);
#endif

        if (retval && insert.execute()) {
            vid = loadVideos(name);
            if (!vid->next()) vt_destruct(vid);
        }
    } while(0);
    
    return vid;
}

ImageFolder* Dataset::createImageFolder(
    const string& name,
    const string& location,
    const string& comment)
{
    ImageFolder *im = NULL;
    
    do {
        string fullpath = _config->baseLocation + _context.datasetLocation + location;

        if (!dirExists(fullpath)) {
            VTLOG_WARNING( "Cannot open folder: " + fullpath);
            break;
        }

        bool retval = true;
        Insert insert(*this, def_tab_sequences);
        retval &= insert.keyString(def_col_seq_name, name);
        retval &= insert.keyString(def_col_seq_location, location);
        retval &= insert.keySeqtype(def_col_seq_type, def_val_images);
        if (!comment.empty()) retval &= insert.keyString(def_col_seq_comment, comment);

        if (retval && insert.execute()) {
            im = loadImageFolders(name);
            if (!im->next()) vt_destruct(im);
        }
    } while (0);
    
    return im;
}

Sequence* Dataset::loadSequences(const string& name)
{
    return (new Sequence(*this, name));
}

Video* Dataset::loadVideos(const string& name)
{
    return (new Video(*this, name));
}

ImageFolder* Dataset::loadImageFolders(const string& name)
{
    return (new ImageFolder(*this, name));
}

Task* Dataset::loadTasks(const string& name)
{
    return (new Task(*this, name));
}

Process* Dataset::loadProcesses(int id)
{
    return (new Process(*this, id));
}


bool Dataset::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_datasets);
    if (ret) {
        ret &= _update->whereString(def_col_ds_name, _context.dataset);
    }
    
    return ret;
}

}
