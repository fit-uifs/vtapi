#include <thread>
#include <chrono>
#include <memory>
#include <Poco/ClassLibrary.h>
#include <vtapi/common/logger.h>
#include "demo1.h"

using namespace std;

namespace vtapi {


Demo1Module::Demo1Module()
{
    _stop = false;
}

Demo1Module::~Demo1Module()
{

}

void Demo1Module::initialize()
{
    VTLOG_DEBUG("demo1: initializing");
}

void Demo1Module::uninitialize()
{
    VTLOG_DEBUG("demo1 : uninitializing");
}

void Demo1Module::process(Process & process)
{
    VTLOG_DEBUG("demo1 : process=" + vtapi::toString<int>(process.getId()));

    {
        shared_ptr<Task> task (process.getParentTask());
        if (task) {
            VTLOG_DEBUG("demo1 : task=" + task->getName());
        }
    }

    {
        shared_ptr<Video> video(process.loadAssignedVideos());
        while(video->next() && !_stop) {
            VTLOG_DEBUG("demo1 : analyzing " + video->getName());
            this_thread::sleep_for(chrono::milliseconds(2500));
        }
    }

    VTLOG_DEBUG("demo1 : processing stopped");
}

void Demo1Module::control(ControlCode code)
{
    switch (code)
    {
    case ControlSuspend:
    case ControlResume:
        break;
    case ControlStop:
        _stop = true;
        break;
    }
}


}


POCO_BEGIN_MANIFEST(vtapi::IModuleInterface)
    POCO_EXPORT_CLASS(vtapi::Demo1Module)
POCO_END_MANIFEST
