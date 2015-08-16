#include <sstream>
#include <Poco/ClassLibrary.h>
#include <vtapi/common/logger.h>
#include "demo2.h"

using namespace std;

namespace vtapi {


Demo2Module::Demo2Module()
{

}

Demo2Module::~Demo2Module()
{

}

bool Demo2Module::initialize()
{
    VTLOG_DEBUG("demo2: initializing");
}

void Demo2Module::uninitialize()
{
    VTLOG_DEBUG("demo2 : uninitializing");
}

void Demo2Module::process(Process & process,
                          Task &task,
                          Video &videos,
                          ImageFolder &imagefolders)
{
    stringstream ss;
    ss << "demo2 : process=" << process.getId() << "; task=" << task.getName();
    VTLOG_DEBUG(ss.str());

    while(videos.next()) {
        VTLOG_DEBUG("demo2 : analyzing " + videos.getName());
    }
}


}


POCO_BEGIN_MANIFEST(vtapi::IModuleInterface)
    POCO_EXPORT_CLASS(vtapi::Demo2Module)
POCO_END_MANIFEST
