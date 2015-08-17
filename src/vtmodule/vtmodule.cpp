#include <memory>
#include <Poco/Exception.h>
#include <Poco/ClassLoader.h>
#include <Poco/Net/TCPServer.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/logger.h>
#include <vtapi/plugins/module_interface.h>
#include <vtapi/vtapi.h>

using namespace vtapi;
using namespace std;


int main(int argc, char *argv[])
{
    int ret = 0;

    try
    {
        VTApi vtapi(argc, argv);

        VTLOG_DEBUG("vtmodule : starting...");

        shared_ptr<Process> prs(vtapi.instantiateProcess());
        if (prs) {
            // get library path
            shared_ptr<Method> met(prs->getParentMethod());
            string libpath = met->getPluginPath();
            VTLOG_DEBUG("vtmodule : loading module from " + libpath);

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

            VTLOG_DEBUG("vtmodule : running module " + plugin_name);

            // run processing
            try
            {
                module->initialize();
                module->process(*prs);
            }
            catch (RuntimeModuleException &e)
            {
                module->uninitialize();
                throw;
            }
            module->uninitialize();
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

//#include <vtapi.h>
//#include <modules/methodinterface.h>

//#include <Poco/ClassLoader.h>
//#include <Poco/Manifest.h>

//using namespace vtapi;

//typedef Poco::ClassLoader<IMethodInterface> PluginLoader;
//typedef Poco::Manifest<IMethodInterface> PluginManifest;

//int main(int argc, char *argv[])
//{
//	PluginLoader loader;

//	//std::string path = std::string("/home/stepo/git/videoterror/vtapi_modules/bin/lib") + argv[1] + ".so";
//	std::cout << argv[1] << std::endl;

//	try
//	{
//		loader.loadLibrary(argv[1]);
//	}
//	catch (Poco::Exception &ex)
//	{
//		std::cout << "Exception message: " << ex.message() << std::endl;
//	}

//	PluginLoader::Iterator it(loader.begin());
//	PluginLoader::Iterator end(loader.end());
//	for (; it != end; ++it)
//	{
//		std::cout << "lib path: " << it->first << std::endl;
//		PluginManifest::Iterator itMan(it->second->begin());
//		PluginManifest::Iterator endMan(it->second->end());
//		for (; itMan != endMan; ++itMan)
//			std::cout << itMan->name() << std::endl;
//	}

//	IMethodInterface* instance = loader.create(loader.begin()->second->begin()->name());
//	std::cout << instance->dummy() << std::endl;
//	delete instance;
//}
