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

#include <Poco/Path.h>
#include <Poco/File.h>
#include <vtapi/common/global.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/insert.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/dataset.h>

using namespace std;

namespace vtapi {


Dataset::Dataset(const Commons& commons, const string& name)
    : KeyValues(commons)
{
    // set the dataset name
    if (!name.empty())
        context().dataset = name;
    
    _select.from(def_tab_datasets, def_col_all);
    _select.orderBy(def_col_ds_name);
    
    if (!context().dataset.empty())
        _select.whereString(def_col_ds_name, context().dataset);
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
        context().dataset = this->getName();
        context().datasetLocation = this->getLocation();
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

string vtapi::Dataset::getDataLocation()
{
    return config().datasets_dir + Poco::Path::separator() +
            this->getLocation();
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

Video* Dataset::createVideo(const string& name,
                            const string& location,
                            const time_t& realtime,
                            double speed,
                            const string& comment)
{
    Video *vid = NULL;

    do {
        string fullpath =
                config().datasets_dir + Poco::Path::separator() +
                context().datasetLocation + Poco::Path::separator() +
                location;

        if (!Poco::Path(fullpath).isFile()) {
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
        
        bool retval = true;
        Insert insert(*this, def_tab_sequences);
        retval &= insert.keyString(def_col_seq_name, name);
        retval &= insert.keyString(def_col_seq_location, location);
        retval &= insert.keySeqtype(def_col_seq_type, def_val_video);
        if (!comment.empty()) retval &= insert.keyString(def_col_seq_comment, comment);
        retval &= insert.keyInt(def_col_seq_vidlength, cnt_frames);
        retval &= insert.keyFloat(def_col_seq_vidfps, fps);
        if (realtime > 0) retval &= insert.keyTimestamp(def_col_seq_vidtime, realtime);
        if (speed > 0.0) retval &= insert.keyFloat(def_col_seq_vidspeed, speed);

        if (retval && insert.execute()) {
            vid = loadVideos(name);
            if (!vid->next()) vt_destruct(vid);
        }
    } while(0);
    
    return vid;
}

ImageFolder* Dataset::createImageFolder(const string& name,
                                        const string& location,
                                        const string& comment)
{
    ImageFolder *im = NULL;
    
    do {
        string fullpath = config().datasets_dir + context().datasetLocation + location;

        if (!Poco::File(Poco::Path(fullpath)).isDirectory()) {
            VTLOG_WARNING("Cannot open folder: " + fullpath);
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

vtapi::Task *Dataset::createTask(const string &mtname,
                                 const TaskParams &params,
                                 const string &prereq_task,
                                 const string &outputs)
{
    Task *ts = NULL;
    string name = Task::constructName(mtname, params);

    // TODO: validace parametru

    QueryTaskCreate q(*this,
                    name,
                    this->getName(),
                    mtname,
                    params.serialize(),
                    prereq_task,
                    outputs);

    if (q.execute()) {
        ts = new Task(*this, name);
        if (!ts->next())
            vt_destruct(ts);
    }

    return ts;
}

Sequence* Dataset::loadSequences(const string& name)
{
    return (new Sequence(*this, name));
}

Sequence *Dataset::loadSequences(const list<string> &names)
{
    return (new Sequence(*this, names));
}

Video* Dataset::loadVideos(const string& name)
{
    return (new Video(*this, name));
}

Video *Dataset::loadVideos(const list<string> &names)
{
    return (new Video(*this, names));
}

ImageFolder* Dataset::loadImageFolders(const string& name)
{
    return (new ImageFolder(*this, name));
}

ImageFolder *Dataset::loadImageFolders(const list<string> &names)
{
    return (new ImageFolder(*this, names));
}

Task* Dataset::loadTasks(const string& name)
{
    return (new Task(*this, name));
}

Task *Dataset::loadTasks(const list<string> &names)
{
    return (new Task(*this, names));
}

Process* Dataset::loadProcesses(int id)
{
    return (new Process(*this, id));
}

Process *Dataset::loadProcesses(const list<int> &ids)
{
    return (new Process(*this, ids));
}

bool vtapi::Dataset::deleteSequence(const string &seqname)
{
    return QuerySequenceDelete(*this, seqname).execute();
}

bool vtapi::Dataset::deleteTask(const string &taskname)
{
    return QueryTaskDelete(*this, this->getName(), taskname).execute();
}


bool Dataset::preUpdate()
{
    bool ret = KeyValues::preUpdate(def_tab_datasets);
    if (ret) {
        ret &= _update->whereString(def_col_ds_name, context().dataset);
    }
    
    return ret;
}

}
