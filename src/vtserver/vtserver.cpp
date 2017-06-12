/*
 * VideoTerror server
 * by: Vojtech Froml (ifroml[at]fit.vutbr.cz)
 */

#include "vtserver.h"
#include <iostream>
#include <rpcz/rpcz.hpp>

#define WORKER_THREAD_COUNT     10
#define ZEROMQ_IO_THREAD_COUNT  1


namespace vti = vtserver_interface;


int main(int argc, char *argv[])
{
    try {
        vtapi::VTApi vtapi(argc, argv);
        vtserver::VTServer vtserver(vtapi);

        rpcz::application::options opts;
        opts.connection_manager_threads = WORKER_THREAD_COUNT;
        opts.zeromq_io_threads = ZEROMQ_IO_THREAD_COUNT;

        rpcz::application app(opts);
        rpcz::server server(app);
        server.register_service(&vtserver);
        server.bind("tcp://*:8719");

        std::cout << "starting server on TCP port 8719..." << std::endl;
        app.run();
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////
//                      VTServer implementation                      //
///////////////////////////////////////////////////////////////////////


namespace vtserver {


VTServer::VTServer(const vtapi::VTApi &vtapi)
    : _connections(WORKER_THREAD_COUNT)
{
    for (auto & conn : _connections)
        conn = std::make_shared<vtapi::VTApi>(vtapi);
}


template<class REQUEST_T, class RESPONSE_T>
bool VTServer::processRequest(REQUEST_T & request, RESPONSE_T & response)
{
    // find current thread index

    int thread_index = -1;
    std::thread::id thread_id = std::this_thread::get_id();

    _mtx_threads.lock();
    auto ti = _thread_indexes.find(thread_id);
    if (ti != _thread_indexes.end()) {
        thread_index = ti->second;
    }
    else {
        thread_index = _thread_indexes.size();
        _thread_indexes.emplace(std::make_pair(thread_id, thread_index));
    }
    _mtx_threads.unlock();

    // get connection for our thread and process request

    if (thread_index >= 0 && thread_index < WORKER_THREAD_COUNT) {
        WorkerJob<REQUEST_T,RESPONSE_T> job(request, response);
        WorkerJobBase::Args args(*_connections[thread_index], _interproc);
        job.process(args);
    }
}


void VTServer::addDataset(const vti::addDatasetRequest &request, ::rpcz::reply<vti::addDatasetResponse> response)
{
    processRequest(request, response);
}

void VTServer::getDatasetList(const vti::getDatasetListRequest &request, ::rpcz::reply<vti::getDatasetListResponse> response)
{
    processRequest(request, response);
}

void VTServer::getDatasetMetrics(const vti::getDatasetMetricsRequest &request, ::rpcz::reply<vti::getDatasetMetricsResponse> response)
{
    processRequest(request, response);
}

void VTServer::deleteDataset(const vti::deleteDatasetRequest &request, ::rpcz::reply<vti::deleteDatasetResponse> response)
{
    processRequest(request, response);
}

void VTServer::addVideo(const vti::addVideoRequest &request, ::rpcz::reply<vti::addVideoResponse> response)
{
    processRequest(request, response);
}

void VTServer::getVideoIDList(const vti::getVideoIDListRequest &request, ::rpcz::reply<vti::getVideoIDListResponse> response)
{
    processRequest(request, response);
}

void VTServer::getVideoInfo(const vti::getVideoInfoRequest &request, ::rpcz::reply<vti::getVideoInfoResponse> response)
{
    processRequest(request, response);
}

void VTServer::setVideoInfo(const vti::setVideoInfoRequest &request, ::rpcz::reply<vti::setVideoInfoResponse> response)
{
    processRequest(request, response);
}

void VTServer::deleteVideo(const vti::deleteVideoRequest &request, ::rpcz::reply<vti::deleteVideoResponse> response)
{
    processRequest(request, response);
}

void VTServer::addTask(const vtserver_interface::addTaskRequest &request, ::rpcz::reply<vtserver_interface::addTaskResponse> response)
{
    processRequest(request, response);
}

void VTServer::getTaskIDList(const vti::getTaskIDListRequest &request, ::rpcz::reply<vti::getTaskIDListResponse> response)
{
    processRequest(request, response);
}

void VTServer::getTaskInfo(const vti::getTaskInfoRequest &request, ::rpcz::reply<vti::getTaskInfoResponse> response)
{
    processRequest(request, response);
}

void VTServer::getTaskProgress(const vti::getTaskProgressRequest &request, ::rpcz::reply<vti::getTaskProgressResponse> response)
{
    processRequest(request, response);
}

void VTServer::deleteTask(const vti::deleteTaskRequest &request, ::rpcz::reply<vti::deleteTaskResponse> response)
{
    processRequest(request, response);
}

void VTServer::getProcessIDList(const vti::getProcessIDListRequest &request, ::rpcz::reply<vti::getProcessIDListResponse> response)
{
    processRequest(request, response);
}

void VTServer::getProcessInfo(const vti::getProcessInfoRequest &request, ::rpcz::reply<vti::getProcessInfoResponse> response)
{
    processRequest(request, response);
}

void VTServer::runProcess(const vti::runProcessRequest &request, ::rpcz::reply<vti::runProcessResponse> response)
{
    processRequest(request, response);
}

void VTServer::stopProcess(const vti::stopProcessRequest &request, ::rpcz::reply<vti::stopProcessResponse> response)
{
    processRequest(request, response);
}

void VTServer::getEventList(const vti::getEventListRequest &request, ::rpcz::reply<vti::getEventListResponse> response)
{
    processRequest(request, response);
}

void VTServer::getEventsStats(const vti::getEventsStatsRequest &request, ::rpcz::reply<vti::getEventsStatsResponse> response)
{
    processRequest(request, response);
}

void VTServer::getProcessingMetadata(const vti::getProcessingMetadataRequest &request, ::rpcz::reply<vti::getProcessingMetadataResponse> response)
{
    processRequest(request, response);
}



}


