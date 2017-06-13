#include <thread>
#include <chrono>
#include <memory>
#include <Poco/ClassLibrary.h>
#include <vtapi/common/logger.h>
#include "demo.h"

using namespace std;

namespace vtapi {


DemoModule::DemoModule()
{
    _stop = false;
}

DemoModule::~DemoModule()
{

}

void DemoModule::install(VTApi &vtapi)
{
//    SELECT public.VT_method_add(
//        'demo1',
//        '{"(features_array,real[],out,,,,)", "(features_mat,public.cvmat,out,,,,)"}',
//        '{"(param1,int,true,50,[-100\\,100],)", "(param2,double,true,0.5,[-1.0\\,1.0],)"}',
//        FALSE,
//        'auto-generated demo method #1',
//        NULL);

    TaskKeyDefinitions keys;
    {
        // TODO: demo key definitions
    }

    TaskParamDefinitions params;
    {
        auto p1 = std::make_shared<TaskParamDefinitionInt>();
        p1->setRequired(true);
        p1->setDefaultValue(50);
        p1->setRange(-100, 100);

        auto p2 = std::make_shared<TaskParamDefinitionDouble>();
        p2->setRequired(true);
        p2->setDefaultValue(0.5);
        p2->setRange(-1.0, 1.0);

        params["param1"] = p1;
        params["param2"] = p2;
    }

    vtapi.createMethod("demo", keys, params, false,
                       "demo method", "empty method for testing/demonstration purposes");
}

void DemoModule::uninstall(VTApi &vtapi)
{
    vtapi.deleteMethod("demo");
}

void DemoModule::initialize(VTApi & vtapi)
{
    VTLOG_MESSAGE("demo : initializing");
}

void DemoModule::uninitialize() noexcept
{
    VTLOG_MESSAGE("demo : uninitializing");
}

void DemoModule::process(Process & process)
{
    VTLOG_MESSAGE("demo : process=" + vtapi::toString<int>(process.getId()));

    {
        shared_ptr<Task> task (process.getParentTask());
        if (task) {
            VTLOG_MESSAGE("demo : task=" + task->getName());
        }
    }

    {
        shared_ptr<Video> video(process.loadAssignedVideos());
        while(video->next() && !_stop) {
            VTLOG_MESSAGE("demo : analyzing " + video->getName());
            this_thread::sleep_for(chrono::milliseconds(2500));
        }
    }

    VTLOG_MESSAGE("demo : processing stopped");
}

void DemoModule::stop() noexcept
{
    _stop = true;
}


}


POCO_BEGIN_MANIFEST(vtapi::IModuleInterface)
    POCO_EXPORT_CLASS(vtapi::DemoModule)
POCO_END_MANIFEST
