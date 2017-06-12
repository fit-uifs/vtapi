/*
 * VideoTerror server
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz)
 */


#pragma once

#include "worker.h"
#include "interproc.h"
#include <vtapi/vtapi.h>
#include "vtserver_interface.rpcz.h"
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

namespace vtserver {


 class VTServer : public vtserver_interface::VTServerInterface
 {
 public:
    VTServer(const vtapi::VTApi &vtapi);

    // VTServerInterface interface

    void addDataset(const vtserver_interface::addDatasetRequest &request, ::rpcz::reply<vtserver_interface::addDatasetResponse> response);
    void getDatasetList(const vtserver_interface::getDatasetListRequest &request, ::rpcz::reply<vtserver_interface::getDatasetListResponse> response);
    void getDatasetMetrics(const vtserver_interface::getDatasetMetricsRequest &request, ::rpcz::reply<vtserver_interface::getDatasetMetricsResponse> response);
    void deleteDataset(const vtserver_interface::deleteDatasetRequest &request, ::rpcz::reply<vtserver_interface::deleteDatasetResponse> response);
    void addVideo(const vtserver_interface::addVideoRequest &request, ::rpcz::reply<vtserver_interface::addVideoResponse> response);
    void getVideoIDList(const vtserver_interface::getVideoIDListRequest &request, ::rpcz::reply<vtserver_interface::getVideoIDListResponse> response);
    void getVideoInfo(const vtserver_interface::getVideoInfoRequest &request, ::rpcz::reply<vtserver_interface::getVideoInfoResponse> response);
    void setVideoInfo(const vtserver_interface::setVideoInfoRequest &request, ::rpcz::reply<vtserver_interface::setVideoInfoResponse> response);
    void deleteVideo(const vtserver_interface::deleteVideoRequest &request, ::rpcz::reply<vtserver_interface::deleteVideoResponse> response);
    void addTask(const vtserver_interface::addTaskRequest &request, ::rpcz::reply<vtserver_interface::addTaskResponse> response);
    void getTaskIDList(const vtserver_interface::getTaskIDListRequest &request, ::rpcz::reply<vtserver_interface::getTaskIDListResponse> response);
    void getTaskInfo(const vtserver_interface::getTaskInfoRequest &request, ::rpcz::reply<vtserver_interface::getTaskInfoResponse> response);
    void getTaskProgress(const vtserver_interface::getTaskProgressRequest &request, ::rpcz::reply<vtserver_interface::getTaskProgressResponse> response);
    void deleteTask(const vtserver_interface::deleteTaskRequest &request, ::rpcz::reply<vtserver_interface::deleteTaskResponse> response);
    void getProcessIDList(const vtserver_interface::getProcessIDListRequest &request, ::rpcz::reply<vtserver_interface::getProcessIDListResponse> response);
    void getProcessInfo(const vtserver_interface::getProcessInfoRequest &request, ::rpcz::reply<vtserver_interface::getProcessInfoResponse> response);
    void runProcess(const vtserver_interface::runProcessRequest &request, ::rpcz::reply<vtserver_interface::runProcessResponse> response);
    void stopProcess(const vtserver_interface::stopProcessRequest &request, ::rpcz::reply<vtserver_interface::stopProcessResponse> response);
    void getEventList(const vtserver_interface::getEventListRequest &request, ::rpcz::reply<vtserver_interface::getEventListResponse> response);
    void getEventsStats(const vtserver_interface::getEventsStatsRequest &request, ::rpcz::reply<vtserver_interface::getEventsStatsResponse> response);
    void getProcessingMetadata(const vtserver_interface::getProcessingMetadataRequest &request, ::rpcz::reply<vtserver_interface::getProcessingMetadataResponse> response);

private:
    std::mutex _mtx_threads;
    std::map<std::thread::id,int> _thread_indexes;
    std::vector< std::shared_ptr<vtapi::VTApi> > _connections;
    Interproc _interproc;

    template<class REQUEST_T, class RESPONSE_T>
    bool processRequest(REQUEST_T & request, RESPONSE_T & reply);    
 };


}
