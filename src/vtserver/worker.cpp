#include "worker.h"
#include "videostats.h"
#include <list>
#include <map>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Exception.h>


using namespace vtapi;
using namespace std;
namespace vti = vtserver_interface;

namespace vtserver {


static const std::string suffix_vidproc("_vp");
static const std::string suffix_evdet("_ed");
static const std::string suffix_procmet("_pm");


vti::Timestamp *WorkerJobBase::createTimestamp(const chrono::system_clock::time_point & value)
{
    chrono::seconds secs = chrono::duration_cast<chrono::seconds>(value.time_since_epoch());
    chrono::nanoseconds nsecs = chrono::duration_cast<chrono::nanoseconds>(value.time_since_epoch());

    vti::Timestamp *ts = new vti::Timestamp();
    ts->set_seconds(secs.count());
    ts->set_nanos(chrono::duration_cast<chrono::nanoseconds>(nsecs - secs).count());

    return ts;
}

chrono::system_clock::time_point WorkerJobBase::fromTimestamp(const vtserver_interface::Timestamp &value)
{
    return chrono::system_clock::from_time_t(value.seconds()) + chrono::nanoseconds(value.nanos());
}

bool WorkerJobBase::parseMethodName(const string &mtname, string &module, vti::taskInfo::taskType &type)
{
    size_t pos;
    if ((pos = mtname.find(suffix_vidproc) != string::npos)) {
        module = mtname.substr(0, pos);
        type = vti::taskInfo::TASK_TYPE_VIDEO_PROCESSING;
        return true;
    }
    else if ((pos = mtname.find(suffix_evdet) != string::npos)) {
        module = mtname.substr(0, pos);
        type = vti::taskInfo::TASK_TYPE_EVENT_DETECTION;
        return true;
    }
    else if ((pos = mtname.find(suffix_procmet) != string::npos)) {
        module = mtname.substr(0, pos);
        type = vti::taskInfo::TASK_TYPE_PROCESSING_METADATA;
        return true;
    }
    else {
        return false;
    }
}

void WorkerJobBase::addTaskParamVT(const vti::taskParam &param,
                                   TaskParams &params)
{
    switch (param.type())
    {
    case vti::taskParam_taskParamType_TP_STRING:
    {
        params.addString(param.name(), param.value_string());
        break;
    }
    case vti::taskParam_taskParamType_TP_INT:
    {
        params.addInt(param.name(), param.value_int());
        break;
    }
    case vti::taskParam_taskParamType_TP_INTARRAY:
    {
        vector<int> values(param.value_int_array_size());
        for (int i = 0; i < param.value_int_array_size(); i++)
            values[i] = param.value_int_array(i);
        params.addIntVector(param.name(), std::move(values));
        break;
    }
    case vti::taskParam_taskParamType_TP_FLOAT:
    {
        params.addDouble(param.name(), param.value_float());
        break;
    }
    case vti::taskParam_taskParamType_TP_FLOATARRAY:
    {
        vector<double> values(param.value_float_array_size());
        for (int i = 0; i < param.value_float_array_size(); i++)
            values[i] = param.value_float_array(i);
        params.addDoubleVector(param.name(), std::move(values));
        break;
    }
    }
}

void WorkerJobBase::addTaskParamGPB(const string& name,
                                    const TaskParamValueBase &param,
                                    vti::taskInfo &info)
{
    vti::taskParam *par = info.add_params();
    par->set_name(name);

    switch (param.getType())
    {
    case TaskParamValueBase::PARAMVALUE_STRING:
    {
        par->set_type(vti::taskParam_taskParamType_TP_STRING);
        par->set_value_string(dynamic_cast< const TaskParamValue<string>& >(param).getValue());
        break;
    }
    case TaskParamValueBase::PARAMVALUE_INT:
    {
        par->set_type(vti::taskParam_taskParamType_TP_INT);
        par->set_value_int(dynamic_cast< const TaskParamValue<int>& >(param).getValue());
        break;
    }
    case TaskParamValueBase::PARAMVALUE_INTVECTOR:
    {
        par->set_type(vti::taskParam_taskParamType_TP_INTARRAY);
        const auto & values = dynamic_cast< const TaskParamValue< vector<int> >& >(param).getValue();
        for (int value : values)
            par->add_value_int_array(value);
        break;
    }
    case TaskParamValueBase::PARAMVALUE_DOUBLE:
    {
        par->set_type(vti::taskParam_taskParamType_TP_FLOAT);
        par->set_value_float(dynamic_cast< const TaskParamValue<double>& >(param).getValue());
        break;
    }
    case TaskParamValueBase::PARAMVALUE_DOUBLEVECTOR:
    {
        par->set_type(vti::taskParam_taskParamType_TP_FLOATARRAY);
        const auto & values = dynamic_cast< const TaskParamValue< vector<double> >& >(param).getValue();
        for (double value : values)
            par->add_value_float_array(value);
        break;
    }
    default:
        break;
    }
}

void WorkerJobBase::parseFilter(const vtserver_interface::eventFilter &filter, vtapi::EventFilter & outfilter)
{
    // filter by duration
    if (filter.has_min_duration() || filter.has_max_duration()) {
        chrono::microseconds min_duration(chrono::microseconds::zero());
        chrono::microseconds max_duration(chrono::microseconds::max());
        if (filter.has_min_duration())
            min_duration = chrono::microseconds(static_cast<chrono::microseconds::rep>(filter.min_duration() * 1000 * 1000));
        if (filter.has_max_duration())
            max_duration = chrono::microseconds(static_cast<chrono::microseconds::rep>(filter.max_duration() * 1000 * 1000));
        outfilter.setDurationFilter(EventFilter::Duration(min_duration, max_duration));
    }

    // filter by time window
    if (filter.has_begin_timewindow() || filter.has_end_timewindow()) {
        chrono::system_clock::time_point begin_window(chrono::microseconds::zero());
        chrono::system_clock::time_point end_window(chrono::system_clock::duration::max());
        if (filter.has_begin_timewindow())
            begin_window = fromTimestamp(filter.begin_timewindow());
        if (filter.has_end_timewindow())
            end_window = fromTimestamp(filter.end_timewindow());
        outfilter.setTimeRangeFilter(EventFilter::TimeRange(begin_window, end_window));
    }

    // filter by day of time
    if (filter.has_begin_daywindow() || filter.has_end_daywindow()) {
        chrono::microseconds begin_window(chrono::microseconds::zero());
        chrono::microseconds end_window(chrono::microseconds::max());
        if (filter.has_begin_daywindow())
            begin_window = chrono::duration_cast<chrono::microseconds>(
                        chrono::seconds(filter.begin_daywindow().seconds()) +
                        chrono::nanoseconds(filter.begin_daywindow().nanos()));
        if (filter.has_end_daywindow())
            end_window = chrono::duration_cast<chrono::microseconds>(
                        chrono::seconds(filter.end_daywindow().seconds()) +
                        chrono::nanoseconds(filter.end_daywindow().nanos()));
        outfilter.setDayTimeRangeFilter(EventFilter::DayTimeRange(begin_window, end_window));
    }

    // filter by overlapping region
    if (filter.has_region()) {
        vtapi::Box region = { filter.region().x1(), filter.region().y1(),
                            filter.region().x2(), filter.region().y2() };
        outfilter.setRegionFilter(region);
    }
}


///////////////////////////////////////////////////////////////////////
//                   RPC methods implementation                      //
///////////////////////////////////////////////////////////////////////

#define VTSERVER_PREPARE_REPLY(type, reply, res) \
    type reply;\
    vti::requestResult *res = new vti::requestResult();\
    res->set_success(false);\
    reply.set_allocated_res(res);

#ifdef VTSERVER_DEBUG
#define VTSERVER_DEBUG_REQUEST  cout << _request.GetTypeName() << " : " <<\
                                _request.ShortDebugString() << endl;
#define VTSERVER_DEBUG_REPLY    cout << reply.ShortDebugString() << endl;
#else
#define VTSERVER_DEBUG_REQUEST
#define VTSERVER_DEBUG_REPLY
#endif


template<>
void WorkerJob<const vti::addDatasetRequest, ::rpcz::reply<vti::addDatasetResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;
    VTSERVER_PREPARE_REPLY(vti::addDatasetResponse, reply, res);

    try
    {
        shared_ptr<Dataset> ds(
            args._vtapi.createDataset(_request.name(), _request.name(),
                                      _request.friendly_name(),
                                      _request.has_description() ? _request.description() : string()));
        if (ds) {
            res->set_success(true);
            reply.set_dataset_id(ds->getName());
        }
        else {
            res->set_msg("Dataset creation failed");
        }
    }
    catch(vtapi::Exception & e)
    {
        res->set_msg(e.message());
    }

    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getDatasetListRequest, ::rpcz::reply<vti::getDatasetListResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getDatasetListResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets();
    if (ds) {
        res->set_success(true);

        while (ds->next()) {
            ::vti::datasetInfo* info = reply.add_datasets();
            info->set_dataset_id(ds->getName());
            info->set_name(ds->getName());
            info->set_friendly_name(ds->getFriendlyName());
            info->set_description(ds->getDescription());
        }

        delete ds;
    }
    else {
        res->set_success(false);
        res->set_msg("Failed to load datasets");
    }

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getDatasetMetricsRequest, ::rpcz::reply<vti::getDatasetMetricsResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getDatasetMetricsResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        res->set_success(true);

        ::vti::datasetMetrics *metrics = new ::vti::datasetMetrics();
        metrics->set_dataset_id(ds->getName());
        reply.set_allocated_metrics(metrics);

        Video *vid = ds->loadVideos();
        metrics->set_video_count(vid->count());
        delete vid;

        Task *ts = ds->loadTasks();
        metrics->set_task_count(ts->count());
        delete ts;

        Process *prs = ds->loadProcesses();
        metrics->set_process_count(prs->count());
        delete prs;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::deleteDatasetRequest, ::rpcz::reply<vti::deleteDatasetResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::deleteDatasetResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        if (args._vtapi.deleteDataset(ds->getName())) {
            res->set_success(true);
            try {
                Poco::File(ds->getDataLocation()).remove(true);
            }
            catch (Poco::Exception &e) {}
        }
        else {
            res->set_success(false);
            res->set_msg("Failed to delete dataset");
        }
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);
}

template<>
void WorkerJob<const vti::addVideoRequest, ::rpcz::reply<vti::addVideoResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::addVideoResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        try {
            string name, location, destpath;

            // get/construct location in dataset
            if (_request.has_location()) {
                location = _request.location();
            }
            else {
                Poco::Path path(_request.filepath());
                location = path.getFileName();
                if (location.empty()) {
                    res->set_msg("Failed to create location from: " + _request.filepath());
                    throw exception();
                }

            }

            // construct full destination path
            string basepath = ds->getDataLocation() + Poco::Path::separator() + location;
            int destindex = 0;
            do {
                if (destindex == 0)
                    destpath = basepath;
                else
                    destpath = basepath + '.' + toString<int>(destindex);
                destindex++;
            } while (Poco::File(destpath).exists());

            // get/construct video name
            if (_request.has_name()) {
                name = _request.name();
            }
            else {
                int nameindex = 0;
                string basename = Poco::Path(basepath).getBaseName();
                do {
                    if (nameindex == 0)
                        name = basename;
                    else
                        name = basename + '.' + toString<int>(nameindex);
                    Sequence *seq = ds->loadSequences(name);
                    nameindex = seq->count() ? nameindex + 1 : 0;
                    delete seq;
                } while(nameindex > 0);
            }

            // copy into dataset, create directory structure
            Poco::File(Poco::Path(destpath).parent()).createDirectories();
            Poco::File(_request.filepath()).copyTo(destpath);

            // validate and insert with VTApi
            chrono::system_clock::time_point start_time;
            if (_request.has_start_time()) {
                start_time = chrono::system_clock::from_time_t(_request.start_time().seconds()) +
                        chrono::nanoseconds(_request.start_time().nanos());
            }
            double speed = _request.has_speed() ? _request.speed() : 0.0;
            string comment = _request.has_comment() ? _request.comment() : string();

            Video *vid = ds->createVideo(name,
                                         location,
                                         start_time,
                                         speed,
                                         comment);
            if (vid) {
                res->set_success(true);
                reply.set_video_id(vid->getName());
                delete vid;
            }
            else {
                res->set_msg("Failed to create video: " + name);
                throw exception();
            }
        }
        catch (Poco::Exception &e)
        {
            res->set_success(false);
            res->set_msg(e.message());
        }
        catch (...)
        {
            res->set_success(false);
            // message set above
        }
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getVideoIDListRequest, ::rpcz::reply<vti::getVideoIDListResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getVideoIDListResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        res->set_success(true);

        Video *vid = ds->loadVideos();
        while (vid->next()) {
            string* info = reply.add_video_ids();
            *info = vid->getName();
        }
        delete vid;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getVideoInfoRequest, ::rpcz::reply<vti::getVideoInfoResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getVideoInfoResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        res->set_success(true);

        // load videos, possibly filter by names
        Video *vid = NULL;
        if (_request.video_ids_size() > 0) {
            vector<string> vidnames(_request.video_ids_size());
            for (int i = 0; i < _request.video_ids_size(); i++)
                vidnames[i] = _request.video_ids(i);
            vid = ds->loadVideos(vidnames);
        }
        else {
            vid = ds->loadVideos();
        }

        // iterate over videos
        while (vid->next()) {
            vti::videoInfo *info = reply.add_videos();
            info->set_video_id(vid->getName());
            info->set_filepath(vid->getDataLocation());
            info->set_location(vid->getLocation());
            info->set_allocated_start_time(createTimestamp(vid->getRealStartTime()));
            info->set_comment(vid->getComment());
            unsigned int length = vid->getLength();
            double fps = vid->getFPS();
            double speed = vid->getSpeed();
            info->set_length_frames(length);
            info->set_fps(fps);
            info->set_speed(speed);
            if (fps > 0)
                info->set_length_ms((length / fps) * 1000 * speed);
            else
                info->set_length_ms(0);
            info->set_allocated_added_time(createTimestamp(vid->getCreatedTime()));
        }
        delete vid;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::setVideoInfoRequest, ::rpcz::reply<vti::setVideoInfoResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::setVideoInfoResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        Video *vid = ds->loadVideos(_request.video_id());
        if (!_request.video_id().empty() && vid->next()) {
            if (_request.has_start_time()) {
                if (vid->updateRealStartTime(fromTimestamp(_request.start_time())) &&
                    vid->updateExecute())
                {
                    res->set_success(true);
                }
                else {
                    res->set_success(false);
                    res->set_msg("Failed to update start_time");
                }
            }
            else {
                res->set_success(false);
                res->set_msg("Video info must have start_time");
            }
        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find video");
        }
        delete vid;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::deleteVideoRequest, ::rpcz::reply<vti::deleteVideoResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::deleteVideoResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        Video *vid = ds->loadVideos(_request.video_id());
        if (!_request.video_id().empty() && vid->next()) {
            if (ds->deleteSequence(vid->getName())) {
                res->set_success(true);
                try
                {
                    Poco::File(vid->getDataLocation()).remove(true);
                }
                catch (Poco::Exception &e) {}
            }
            else {
                res->set_msg("Failed to delete video " + vid->getName());
                res->set_success(false);
            }

        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find video");
        }
        delete vid;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::addTaskVideoProcessingRequest, ::rpcz::reply<vti::addTaskVideoProcessingResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::addTaskVideoProcessingResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        TaskParams params;
        for (int i = 0; i < _request.params_size(); i++)
            addTaskParamVT(_request.params(i), params);

        string mtname = _request.module() + suffix_vidproc;

        // check if task already exists, if not create it
        string taskname = Task::constructName(mtname, params);
        Task *ts = ds->loadTasks(taskname);
        if (!ts->next()) {
            delete ts;
            ts = ds->createTask(mtname,
                                params,
                                string(),
                                string());
        }
        if (ts) {
            res->set_success(true);
            reply.set_task_id(ts->getName());
            delete ts;
        }
        else {
            res->set_success(false);
            res->set_msg("Failed to create task");
        }
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::addTaskEventDetectionRequest, ::rpcz::reply<vti::addTaskEventDetectionResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::addTaskEventDetectionResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        TaskParams params;
        for (int i = 0; i < _request.params_size(); i++)
            addTaskParamVT(_request.params(i), params);

        string mtname = _request.module() + suffix_evdet;

        // check if task already exists, if not create it
        string taskname = Task::constructName(mtname, params);
        Task *ts = ds->loadTasks(taskname);
        if (!ts->next()) {
            delete ts;
            ts = ds->createTask(mtname,
                                params,
                                _request.prereq_task_id(),
                                string());
        }
        if (ts) {
            res->set_success(true);
            reply.set_task_id(ts->getName());
            delete ts;
        }
        else {
            res->set_success(false);
            res->set_msg("Failed to create task");
        }
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::addTaskProcessingMetadataRequest, ::rpcz::reply<vti::addTaskProcessingMetadataResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::addTaskProcessingMetadataResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        TaskParams params;
        for (int i = 0; i < _request.params_size(); i++)
            addTaskParamVT(_request.params(i), params);

        string mtname = _request.module() + suffix_procmet;

        // check if task already exists, if not create it
        string taskname = Task::constructName(mtname, params);
        Task *ts = ds->loadTasks(taskname);
        if (!ts->next()) {
            delete ts;
            ts = ds->createTask(mtname,
                                params,
                                _request.prereq_task_id(),
                                string());
        }
        if (ts) {
            res->set_success(true);
            reply.set_task_id(ts->getName());
            delete ts;
        }
        else {
            res->set_success(false);
            res->set_msg("Failed to create task");
        }
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getTaskIDListRequest, ::rpcz::reply<vti::getTaskIDListResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getTaskIDListResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        res->set_success(true);

        Task *ts = ds->loadTasks();
        while (ts->next()) {
            string* info = reply.add_task_ids();
            *info = ts->getName();
        }
        delete ts;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}



template<>
void WorkerJob<const vti::getTaskInfoRequest, ::rpcz::reply<vti::getTaskInfoResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getTaskInfoResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (ds->next()) {
        res->set_success(true);

        // load tasks, possibly filter by names
        Task *ts = NULL;
        if (_request.task_ids_size() > 0) {
            vector<string> tsnames(_request.task_ids_size());
            for (int i = 0; i < _request.task_ids_size(); i++)
                tsnames[i] = _request.task_ids(i);
            ts = ds->loadTasks(tsnames);
        }
        else {
            ts = ds->loadTasks();
        }

        // iterate over tasks
        while (ts->next()) {
            vti::taskInfo *info = reply.add_tasks();
            info->set_task_id(ts->getName());

            // infer module + type from method name
            string mtname = ts->getParentMethodName();
            string module;
            vti::taskInfo::taskType type;
            if (parseMethodName(mtname, module, type)) {
                info->set_module(module);
                info->set_task_type(type);
            }
            else {
                info->set_module(mtname);
                info->set_task_type(vti::taskInfo::TASK_TYPE_UNKNOWN);
            }

            // construct params
            TaskParams params = ts->getParams();
            for (const auto & param : params.data())
                addTaskParamGPB(param.first, *param.second, *info);

            // prerequisite task
            Task *ts_prereq = ts->loadPrerequisiteTasks();
            if (ts_prereq->next()) {
                info->set_prereq_task_id(ts_prereq->getName());
            }
            delete ts_prereq;

            // processes run on this task
            Process *prs = ts->loadProcesses();
            while (prs->next()) {
                string *prsid = info->add_process_ids();
                *prsid = vtapi::toString<int>(prs->getId());
            }
            delete prs;

            // added time
            vti::Timestamp *added = this->createTimestamp(ts->getCreatedTime());
            info->set_allocated_added_time(added);

        }
        delete ts;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getTaskProgressRequest, ::rpcz::reply<vti::getTaskProgressResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getTaskProgressResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        // video names list
        vector<string> vidnames(_request.video_ids_size());
        for (int i = 0; i < _request.video_ids_size(); i++)
            vidnames[i] = _request.video_ids(i);

        unsigned int vids_count = 0;
        unsigned long long vids_total_length = 0;
        unsigned long long vids_partial_length = 0;
        unsigned long long vids_done_length = 0;
        map<string,unsigned int> vids_lengths_map;
        map<int,string> processes_vids_map;

        // get requested videos and their lengths
        Video *vids;
        if (vidnames.size() > 0)
            vids = ds->loadVideos(vidnames);
        else
            vids = ds->loadVideos();
        while (vids->next()) {
            unsigned int length = vids->getLength();
            vids_count++;
            vids_total_length += length;
            vids_lengths_map[vids->getName()] = length;
        }

        // find requested task
        Task *ts = ds->loadTasks(_request.task_id());
        if (!_request.task_id().empty() && ts->next()) {
            res->set_success(true);

            vti::taskProgress *tp = new vti::taskProgress();
            reply.set_allocated_task_progress(tp);

            // load progress info for all or specified videos
            TaskProgress *tprog;
            if (vidnames.size() > 0)
                tprog = ts->loadTaskProgress(vidnames);
            else
                tprog = ts->loadTaskProgress();

            // iterate over progress info
            while (tprog->next()) {
                string vidname = tprog->getSequenceName();
                if (tprog->getIsDone()) {
                    const auto it = vids_lengths_map.find(vidname);
                    if (it != vids_lengths_map.end())
                        vids_done_length += it->second;

                    tp->add_done_video_ids(vidname);
                }
                else {
                    tp->add_inprogress_video_ids(vidname);
                    processes_vids_map[tprog->getProcessId()] = vidname;
                }
            }

            // calculate partial progress for videos in progress
            if (!processes_vids_map.empty() > 0) {
                vector<int> prsids(processes_vids_map.size());
                int idx = 0;
                for (auto &it : processes_vids_map)
                    prsids[idx++] = it.first;

                Process *prs = ds->loadProcesses(prsids);
                while (prs->next()) {
                    // get length of processed video
                    unsigned long long vid_length = 0;
                    auto prsvid = processes_vids_map.find(prs->getId());
                    if (prsvid != processes_vids_map.end()) {
                        auto vidlen = vids_lengths_map.find(prsvid->second);
                        if (vidlen != vids_lengths_map.end())
                            vid_length = vidlen->second;
                    }

                    // get process state and partial progress
                    if (vid_length > 0) {
                        ProcessState state = prs->getState();
                        if (state.status == ProcessState::STATUS_RUNNING)
                            vids_partial_length += static_cast<unsigned long long>(state.progress * vid_length);
                        else if (state.status == ProcessState::STATUS_FINISHED)
                            vids_partial_length += vid_length;
                    }
                }
                delete prs;
            }

            if (vids_total_length > 0)
                tp->set_progress(static_cast<double>(vids_done_length + vids_partial_length) / vids_total_length);
            else
                tp->set_progress(0);

            // TODO: estimated time?

            delete tprog;
        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find task");
        }
        delete ts;
        delete vids;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;


    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::deleteTaskRequest, ::rpcz::reply<vti::deleteTaskResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::deleteTaskResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        if (ds->deleteTask(_request.task_id())) {
            res->set_success(true);
        }
        else {
            res->set_success(false);
            res->set_msg("Failed to delete task");
        }
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getProcessIDListRequest, ::rpcz::reply<vti::getProcessIDListResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getProcessIDListResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        res->set_success(true);

        Process *prs = ds->loadProcesses();
        while (prs->next()) {
            string* info = reply.add_process_ids();
            *info = toString<int>(prs->getId());
        }
        delete prs;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getProcessInfoRequest, ::rpcz::reply<vti::getProcessInfoResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getProcessInfoResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        res->set_success(true);

        // load processes, possibly filter by IDs
        Process *prs = NULL;
        if (_request.process_ids_size() > 0) {
            vector<int> prsids(_request.process_ids_size());
            for (int i = 0; i < _request.process_ids_size(); i++)
                prsids[i] = std::atoi(_request.process_ids(i).c_str());
            prs = ds->loadProcesses(prsids);
        }
        else {
            prs = ds->loadProcesses();
        }

        // iterate over processes
        while (prs->next()) {
            vti::processInfo *info = reply.add_processes();
            info->set_process_id(toString<int>(prs->getId()));
            info->set_assigned_task_id(prs->getParentTaskName());

            Video *vids = prs->loadAssignedVideos();
            while(vids->next()) {
                string *vidname = info->add_assigned_video_ids();
                *vidname = vids->getName();
            }
            delete vids;

            ProcessState state = prs->getState();
            switch (state.status)
            {
            case ProcessState::STATUS_CREATED:
                info->set_state(vti::processInfo_processState_STATE_CREATED);
                break;
            case ProcessState::STATUS_RUNNING:
                info->set_state(vti::processInfo_processState_STATE_RUNNING);
                info->set_current_item(state.current_item);
                info->set_progress(state.progress);
                break;
            case ProcessState::STATUS_FINISHED:
                info->set_state(vti::processInfo_processState_STATE_FINISHED);
                break;
            case ProcessState::STATUS_ERROR:
                info->set_state(vti::processInfo_processState_STATE_RUNNING);
                info->set_error_message(state.last_error);
                info->set_progress(state.progress);
                break;
            }
        }
        delete prs;

    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::runProcessRequest, ::rpcz::reply<vti::runProcessResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::runProcessResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        Task *ts = ds->loadTasks(_request.task_id());
        if (!_request.task_id().empty() && ts->next()) {
            vector<string> seqnames(_request.video_ids_size());
            for (int i = 0; i < _request.video_ids_size(); i++)
                seqnames[i] = _request.video_ids(i);

            Process *prs = ts->createProcess(seqnames);
            if (prs) {
                vtapi::InterProcessClient *ipc = prs->launchInstance();
                if (ipc) {
                    args._ipc.addClientInstance(ipc);
                    res->set_success(true);
                    reply.set_process_id(toString<int>(prs->getId()));
                }
                else {
                    res->set_success(false);
                    res->set_msg("Failed to launch process instance");
                }
                delete prs;
            }
            else {
                res->set_success(false);
                res->set_msg("Failed to create process");
            }
        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find task");
        }
        delete ts;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::stopProcessRequest, ::rpcz::reply<vti::stopProcessResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::stopProcessResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (!_request.dataset_id().empty() && ds->next()) {
        Process *prs = ds->loadProcesses(std::atoi(_request.process_id().c_str()));
        if (!_request.process_id().empty() && prs->next()) {
            InterProcessClient *ipc = prs->connectToInstance();
            if (ipc) {
                ipc->stop();    // may block for 0,5s
                delete ipc;
                res->set_success(true);
            }
            else {
                res->set_success(false);
                res->set_msg("Cannot connect to process instance");
            }
        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find process");
        }
        delete prs;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getEventListRequest, ::rpcz::reply<vti::getEventListResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getEventListResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (ds->next()) {
        Task *ts =  ds->loadTasks(_request.task_id());
        if (ts->next()) {
            res->set_success(true);

            // map added videos to their infos
            map<string,vti::eventInfoList*> vid_infos;
            // map trajectories to their events
            map<int,vti::eventInfo*> trajectories;

            // load intervals, possibly filter by sequences
            Interval *outdata = ts->loadOutputData();
            vector<string> seqnames;
            if (_request.video_ids_size() > 0) {
                seqnames.resize(_request.video_ids_size());
                for (int i = 0; i < _request.video_ids_size(); i++)
                    seqnames[i] = _request.video_ids(i);
                outdata->filterBySequences(seqnames);
            }

            // apply filters
            if (_request.has_filter()) {
                EventFilter flt;
                parseFilter(_request.filter(), flt);
                outdata->filterByEvent("event", _request.task_id(), seqnames, flt);
            }

            // iterate over events
            while (outdata->next()) {
                // get video info
                vti::eventInfoList *info;
                string vidname = outdata->getParentSequenceName();
                auto it = vid_infos.find(vidname);
                if (it == vid_infos.end()) {
                    info = reply.add_events_list();
                    info->set_video_id(vidname);
                    vid_infos.insert(std::make_pair(vidname,info));
                }
                else {
                    info = (*it).second;
                }

                // get output event
                IntervalEvent ev = outdata->getIntervalEvent("event");
                int t1 = outdata->getStartTime();
                int t2 = outdata->getEndTime();
                double to_sec = t2+1 > t1 ? (outdata->getLengthSeconds() / ((t2+1) - t1)) : 0;

                // add new trajectory root (or non-trajectory event)
                if (ev.is_root) {
                    if (trajectories.find(ev.group_id) == trajectories.end()) {
                        vti::eventInfo *traj = info->add_events();
                        trajectories.insert(std::make_pair(ev.group_id, traj));
                        traj->set_group_id(ev.group_id);
                        traj->set_class_id(ev.class_id);
                        traj->set_score(ev.score);
                        traj->set_t1(t1);
                        traj->set_t2(t2);
                        traj->set_t1_sec(t1*to_sec);
                        traj->set_t2_sec((t2+1)*to_sec);
                    }
                }
                // add event to existing trajectory
                else {
                    auto it = trajectories.find(ev.group_id);
                    if (it != trajectories.end()) {
                        vti::Region *reg = it->second->add_regions();
                        reg->set_t(t1);
                        reg->set_t_sec(t1*to_sec);
                        reg->set_x1(ev.region.high.x);
                        reg->set_x2(ev.region.low.x);
                        reg->set_y1(ev.region.high.y);
                        reg->set_y2(ev.region.low.y);
                    }
                }
            }
            delete outdata;
        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find task");
        }
        delete ts;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getEventsStatsRequest, ::rpcz::reply<vti::getEventsStatsResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getEventsStatsResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (ds->next()) {
        Task *ts =  ds->loadTasks(_request.task_id());
        if (ts->next()) {
            res->set_success(true);

            // load intervals, possibly filter by sequences
            Interval *outdata = ts->loadOutputData();
            Video *vids = NULL;
            vector<string> seqnames;
            if (_request.video_ids_size() > 0) {
                seqnames.resize(_request.video_ids_size());
                for (int i = 0; i < _request.video_ids_size(); i++)
                    seqnames[i] = _request.video_ids(i);
                outdata->filterBySequences(seqnames);
                vids = ds->loadVideos(seqnames);
            }
            else {
                vids = ds->loadVideos();
            }

            // map added videos to their stats
            struct vid_item
            {
                vti::eventStats* stats_out;
                VideoStats stats_int;
            };
            map<string,vid_item> vids_map;

            // initialize stats structures for all videos
            while (vids->next()) {
                vid_item item = { reply.add_stats(), VideoStats(vids->getLength()) };
                vids_map.insert(make_pair(vids->getName(), std::move(item)));
            }
            delete vids;

            // apply filters
            if (_request.has_filter()) {
                EventFilter flt;
                parseFilter(_request.filter(), flt);
                outdata->filterByEvent("event", _request.task_id(), seqnames, flt);
            }

            // iterate over events
            while (outdata->next()) {
                auto item = vids_map.find(outdata->getParentSequenceName());
                // all videos should have stats prepared
                if (item != vids_map.end()) {
                    item->second.stats_int.processEvent(outdata->getStartTime(),
                                                        outdata->getEndTime(),
                                                        outdata->getIntervalEvent("event"));

                }
            }
            delete outdata;

            // fill stats
            for (auto item : vids_map) {
                item.second.stats_out->set_video_id(item.first);
                item.second.stats_out->set_count(item.second.stats_int.count_root());
                item.second.stats_out->set_coverage(item.second.stats_int.calculateCoverage());
            }
        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find task");
        }
        delete ts;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}

template<>
void WorkerJob<const vti::getProcessingMetadataRequest, ::rpcz::reply<vti::getProcessingMetadataResponse> >
::process(Args & args)
{
    VTSERVER_DEBUG_REQUEST;

    vti::getProcessingMetadataResponse reply;
    vti::requestResult *res = new vti::requestResult();

    Dataset *ds = args._vtapi.loadDatasets(_request.dataset_id());
    if (ds->next()) {
        Task *ts =  ds->loadTasks(_request.task_id());
        if (ts->next()) {
            res->set_success(true);

            // total keyframes count
            unsigned int total_keyframe_count = 0;
            // class ID occurrences sums
            map<int,double> sum_occurences;

            // load class ID occurrences for all videos
            Interval *outdata = ts->loadOutputData();
            vector<string> seqnames;
            if (_request.video_ids_size() > 0) {
                seqnames.resize(_request.video_ids_size());
                for (int i = 0; i < _request.video_ids_size(); i++)
                    seqnames[i] = _request.video_ids(i);
                outdata->filterBySequences(seqnames);
            }

            // iterate over class ID records
            while (outdata->next()) {
                int class_id = outdata->getInt("out_class_id");
                double occur = outdata->getFloat8("out_occurrence");
                // class_id -1 is special value - keyframe count
                if (class_id == -1) {
                    total_keyframe_count += static_cast<unsigned int>(occur);
                }
                else {
                    auto it = sum_occurences.find(class_id);
                    if (it != sum_occurences.end()) {
                        it->second += occur;
                    }
                    else {
                        sum_occurences[class_id] = occur;
                    }
                }
            }
            delete outdata;

            // total occurrence rate of classes
            class class_occurrence
            {
            public:
                int _id;
                double _occurrence;

                class_occurrence()
                    : _id(-1), _occurrence(0) {}
                class_occurrence(int id, double occ)
                    : _id(id), _occurrence(occ) {}
                bool operator< (const class_occurrence &cmp) const
                { return _occurrence > cmp._occurrence; }
            };
            vector<class_occurrence> rate_occurrences(sum_occurences.size());

            // sum, calculate and sort rates
            auto itrate = rate_occurrences.begin();
            for (auto itsum : sum_occurences) {
                (*itrate++) = class_occurrence(itsum.first, itsum.second / total_keyframe_count);
            }
            std::sort(rate_occurrences.begin(), rate_occurrences.end());

            // construct reply
            vti::processingMetadataVideoType *md = new vti::processingMetadataVideoType();
            reply.set_allocated_metadata_videotype(md);
            for (auto & item : rate_occurrences) {
                vti::classIdOccurence *occ = md->add_class_id_occurence();
                occ->set_class_id(item._id);
                occ->set_occurrence(item._occurrence);
            }
        }
        else {
            res->set_success(false);
            res->set_msg("Cannot find task");
        }
        delete ts;
    }
    else {
        res->set_success(false);
        res->set_msg("Cannot find dataset");
    }
    delete ds;

    reply.set_allocated_res(res);
    _response.send(reply);

    VTSERVER_DEBUG_REPLY;
}


}
