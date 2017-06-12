#pragma once

#include "interproc.h"
#include <vtapi/vtapi.h>
#include "vtserver_interface.rpcz.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <chrono>

namespace vtserver {


class WorkerJobBase
{
public:
    class Args
    {
    public:
        vtapi::VTApi & _vtapi;
        Interproc & _ipc;

        Args(vtapi::VTApi & vtapi, Interproc & ipc)
            : _vtapi(vtapi), _ipc(ipc) {}
    };

public:
    virtual void process(Args & args) = 0;

protected:
    ::vtserver_interface::Timestamp *createTimestamp(
            const std::chrono::system_clock::time_point & value);

    std::chrono::system_clock::time_point fromTimestamp(
            const ::vtserver_interface::Timestamp &value);

    bool parseMethodName(const std::string& mtname,
                         std::string &module,
                         vtserver_interface::taskInfo::taskType &type);

    void addTaskParamVT(const vtserver_interface::taskParam &param,
                        vtapi::TaskParams &params);

    void addTaskParamGPB(const std::string& name,
                         const vtapi::TaskParamValueBase &param,
                         vtserver_interface::taskInfo &info);

    void parseFilter(const vtserver_interface::eventFilter &filter,
                     vtapi::EventFilter & outfilter);
};

template<class REQUEST_T, class RESPONSE_T>
class WorkerJob : public WorkerJobBase
{
public:
    WorkerJob(REQUEST_T & request, RESPONSE_T &response)
        : _request(request), _response(response) {}
    void process(Args & args) override;

private:
    REQUEST_T & _request;
    RESPONSE_T & _response;
};

}
