#include <sstream>
#include <Poco/ClassLibrary.h>
#include <vtapi/common/logger.h>
#include "demo1.h"

using namespace std;

namespace vtapi {


Demo1Module::Demo1Module()
{

}

Demo1Module::~Demo1Module()
{

}

bool Demo1Module::initialize()
{
    VTLOG_DEBUG("demo1 : initializing");
}

void Demo1Module::uninitialize()
{
    VTLOG_DEBUG("demo1 : uninitializing");
}

void Demo1Module::process(Process & process,
                          Task &task,
                          Video &videos,
                          ImageFolder &imagefolders)
{
    stringstream ss;
    ss << "demo1 : process=" << process.getId() << "; task=" << task.getName();
    VTLOG_DEBUG(ss.str());

    while(videos.next()) {
        VTLOG_DEBUG("demo1 : analyzing " + videos.getName());
    }
}


}


POCO_BEGIN_MANIFEST(vtapi::IModuleInterface)
    POCO_EXPORT_CLASS(vtapi::Demo1Module)
POCO_END_MANIFEST
