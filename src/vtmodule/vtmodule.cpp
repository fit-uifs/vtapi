#include "vtmodule.h"
#include <vtapi/common/exception.h>
#include <vtapi/common/logger.h>
#include <vtapi/vtapi.h>
#include <memory>
#include <Poco/Exception.h>
#include <Poco/ClassLoader.h>
#include <Poco/Process.h>



using namespace std;


int main(int argc, char *argv[])
{
    return vtapi::VTModule().main(argc, argv);
}


namespace vtapi {


int VTModule::main(int argc, char *argv[])
{
    int ret = 0;

    try
    {
        VTApi vtapi(argc, argv);

        VTLOG_DEBUG("vtmodule : starting...");

        shared_ptr<Process> prs(vtapi.getRunnableProcess());

        if (prs) {
            // get library path
            shared_ptr<Method> met(prs->getParentMethod());
            string libpath = met->getPluginPath();
            VTLOG_DEBUG("vtmodule : loading module from: " + libpath);

            Poco::ClassLoader<IModuleInterface> loader;
            shared_ptr<IModuleInterface> module;
            string plugin_name;

            try
            {
                // load library
                loader.loadLibrary(libpath);

                // load plugin interface
                auto & plugins = *loader.begin()->second;
                for (const auto & plugin : plugins) {
                    plugin_name = plugin->name();
                    break;  // get first plugin
                }
                if (plugin_name.empty())
                    throw(ModuleException(libpath, "library is not a VTApi module plugin"));

                module = shared_ptr<IModuleInterface>(loader.create(plugin_name));
            }
            catch (Poco::Exception &e)
            {
                throw ModuleException(libpath, e.message());
            }

            VTLOG_DEBUG("vtmodule : running module: " + plugin_name);

            {
                // helper object for passing control signals to module
                ModuleControl control(*module);

                // server will listen for commands
                shared_ptr<InterProcessServer> srv(prs->initializeInstance(control));

                // run processing
                try
                {
                    module->initialize(vtapi);
                    module->process(*prs);
                    module->uninitialize();
                }
                catch (Exception &e)
                {
                    module->uninitialize();
                    throw;
                }
            }
        }
        else {
            throw ModuleException("<unknown>", "failed to instantiate process");
        }
    }
    catch (Exception &e)
    {
        VTLOG_ERROR("vtmodule : " + e.message());
        ret = 1;
    }

    VTLOG_DEBUG("vtmodule : stopped");

    return ret;
}


}
