#include <iostream>
#include <Poco/Exception.h>
#include <Poco/ClassLoader.h>
#include <Poco/Net/TCPServer.h>
#include <vtapi/common/logger.h>
#include <vtapi/plugins/module_interface.h>
#include <vtapi/vtapi.h>

using namespace vtapi;
using namespace std;

int main(int argc, char *argv[])
{
    VTLOG_DEBUG("vtmodule : starting...");

    VTApi vtapi(argc, argv);

    Process *prs = vtapi.instantiateProcess();
    if (prs) {
        // get library path
        Method *met = prs->getParentMethod();
        string libpath = met->getPluginPath();
        delete met;

        VTLOG_DEBUG("vtmodule loading plugin : " + libpath);

        try {
            // load library
            Poco::ClassLoader<IModuleInterface> loader;
            loader.loadLibrary(libpath);

            // load plugin interface
            string plugin_name = loader.begin()->second->begin()->name();
            IModuleInterface *pmodule = loader.create(plugin_name);

            // run processing
            if (pmodule) {
                if (pmodule->initialize()) {
                    Task *task = prs->getParentTask();
                    if (task) {
                        Video *vid = prs->loadAssignedVideos();
                        ImageFolder *imf = prs->loadAssignedImageFolders();

                        pmodule->process(*prs, *task, *vid, *imf);

                        delete imf;
                        delete vid;

                        delete task;
                    }
                }
                pmodule->uninitialize();

                delete pmodule;
            }
        }
        catch (Poco::Exception &e) {
             VTLOG_ERROR("vtmodule : " + e.message());
        }

        delete prs;
    }
    else {
        VTLOG_ERROR("vtmodule : failed to instantiate process");
    }

    VTLOG_DEBUG("vtmodule : stopped");

    return 0;
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
