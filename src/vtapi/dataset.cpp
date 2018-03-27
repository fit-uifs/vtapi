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

#include <Poco/DirectoryIterator.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <vtapi/common/global.h>
#include <vtapi/common/defs.h>
#include <vtapi/queries/insert.h>
#include <vtapi/queries/delete.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/data/dataset.h>

using namespace std;

namespace vtapi {


Dataset::Dataset(const Dataset &copy)
    : Dataset(dynamic_cast<const Commons&>(copy))
{
}

Dataset::Dataset(const Commons& commons, const string& name)
    : KeyValues(commons, def_tab_datasets)
{
    // set the dataset name
    if (!name.empty())
        _context.dataset = name;
    
    _select.setOrderBy(def_col_ds_name);
    
    if (!_context.dataset.empty())
        _select.querybuilder().whereString(def_col_ds_name, _context.dataset);
}

Dataset::Dataset(const Commons& commons, const vector<string>& names)
    : KeyValues(commons, def_tab_datasets)
{
    _select.setOrderBy(def_col_ds_name);
    _select.querybuilder().whereStringVector(def_col_ds_name, names);
}

bool Dataset::next()
{
    if (KeyValues::next()) {
        _context.dataset = this->getName();
        _context.dataset_location = this->getLocation();
        return true;
    }
    else {
        return false;
    }
}

string Dataset::getName() const
{
    return this->getString(def_col_ds_name);
}

string Dataset::getFriendlyName() const
{
    return this->getString(def_col_ds_fname);
}

string Dataset::getLocation() const
{
    return this->getString(def_col_ds_location);
}

string Dataset::getDescription() const
{
    return this->getString(def_col_ds_description);
}

string vtapi::Dataset::getDataLocation() const
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
    const string& comment) const
{
    Sequence *seq = NULL;

    bool retval = true;
    Insert insert(*this, def_tab_sequences);
    retval &= insert.querybuilder().keyString(def_col_seq_name, name);
    retval &= insert.querybuilder().keyString(def_col_seq_location, location);
    retval &= insert.querybuilder().keySeqtype(def_col_seq_type, type);
    if (!comment.empty()) retval &= insert.querybuilder().keyString(def_col_seq_comment, comment);
    
    if (retval && insert.execute()) {
        seq = loadSequences(name);
        if (!seq->next()) vt_destruct(seq);
    }

    return seq;
}

Video* Dataset::createVideo(const string& name,
                            const string& location,
                            const chrono::system_clock::time_point& realtime,
                            double speed,
                            const string& comment) const
{
    Video *vid = NULL;

    do {
        string fullpath =
                config().datasets_dir + Poco::Path::separator() +
                _context.dataset_location + Poco::Path::separator() +
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
        retval &= insert.querybuilder().keyString(def_col_seq_name, name);
        retval &= insert.querybuilder().keyString(def_col_seq_location, location);
        retval &= insert.querybuilder().keySeqtype(def_col_seq_type, def_val_video);
        if (!comment.empty())
            retval &= insert.querybuilder().keyString(def_col_seq_comment, comment);
        retval &= insert.querybuilder().keyInt(def_col_seq_length, cnt_frames);
        retval &= insert.querybuilder().keyFloat(def_col_seq_vidfps, fps);
        if (realtime.time_since_epoch() > chrono::seconds(0))
            retval &= insert.querybuilder().keyTimestamp(def_col_seq_vidtime, realtime);
        if (speed > 0.0)
            retval &= insert.querybuilder().keyFloat(def_col_seq_vidspeed, speed);

        if (retval && insert.execute()) {
            vid = loadVideos(name);
            if (!vid->next()) vt_destruct(vid);
        }
    } while(0);
    
    return vid;
}

ImageFolder* Dataset::createImageFolder(const string& name,
                                        const string& location,
                                        const chrono::system_clock::time_point& realtime,
                                        const string& comment) const
{
    ImageFolder *im = NULL;
    
    do {
        string fullpath = config().datasets_dir + Poco::Path::separator() + _context.dataset_location + Poco::Path::separator() + location;

        if (!Poco::File(Poco::Path(fullpath)).isDirectory()) {
            VTLOG_WARNING("Cannot open folder: " + fullpath);
            break;
        }
        
        int cnt_images = 0;
        Poco::DirectoryIterator end;
        for (Poco::DirectoryIterator it(fullpath); it != end; ++it) {
            cv::Mat image;
            
            if (! it->isFile()) {
                VTLOG_WARNING("Unexpected filesystem structure (maybe nested directory?): " + it->path());
                cnt_images = -1;
                break;
            }
            
            image = cv::imread(it->path());
            if (! image.data) {
                VTLOG_WARNING("Invalid image file: " + it->path());
                cnt_images = -1;
                break;
            }
            
            cnt_images++;
        }
        
        if (cnt_images == -1) {
            break;
        }
        else if (cnt_images == 0) {
            VTLOG_WARNING("Image path does not contain any image: " + fullpath);
            break;
        }

        bool retval = true;
        Insert insert(*this, def_tab_sequences);
        retval &= insert.querybuilder().keyString(def_col_seq_name, name);
        retval &= insert.querybuilder().keyString(def_col_seq_location, location);
        retval &= insert.querybuilder().keySeqtype(def_col_seq_type, def_val_images);
        retval &= insert.querybuilder().keyInt(def_col_seq_length, cnt_images);
        if (realtime.time_since_epoch() > chrono::seconds(0))
            retval &= insert.querybuilder().keyTimestamp(def_col_seq_vidtime, realtime);
        if (!comment.empty()) retval &= insert.querybuilder().keyString(def_col_seq_comment, comment);

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
                                 const string &outputs) const
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

Sequence* Dataset::loadSequences(const string& name) const
{
    return (new Sequence(*this, name));
}

Sequence *Dataset::loadSequences(const vector<string> &names) const
{
    return (new Sequence(*this, names));
}

Video* Dataset::loadVideos(const string& name) const
{
    return (new Video(*this, name));
}

Video *Dataset::loadVideos(const vector<string> &names) const
{
    return (new Video(*this, names));
}

ImageFolder* Dataset::loadImageFolders(const string& name) const
{
    return (new ImageFolder(*this, name));
}

ImageFolder *Dataset::loadImageFolders(const vector<string> &names) const
{
    return (new ImageFolder(*this, names));
}

Task* Dataset::loadTasks(const string& name) const
{
    return (new Task(*this, name));
}

Task *Dataset::loadTasks(const vector<string> &names) const
{
    return (new Task(*this, names));
}

Process* Dataset::loadProcesses(int id) const
{
    return (new Process(*this, id));
}

Process *Dataset::loadProcesses(const vector<int> &ids) const
{
    return (new Process(*this, ids));
}

bool Dataset::deleteSequence(const string &seqname) const
{
    Delete d(*this, def_tab_sequences);
    return d.querybuilder().whereString(def_col_seq_name, seqname) && d.execute();
}

bool Dataset::deleteTask(const string &taskname) const
{
    return QueryTaskDelete(*this, this->getName(), taskname).execute();
}


bool Dataset::preUpdate()
{
    return update().querybuilder().whereString(def_col_ds_name, _context.dataset);
}

}
