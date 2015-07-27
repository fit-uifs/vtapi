#include <vtapi.h>
#include <modules/vtapi_methodinterface.h>

#include <Poco/ClassLoader.h>
#include <Poco/Manifest.h>

using namespace vtapi;

typedef Poco::ClassLoader<IMethodInterface> PluginLoader;
typedef Poco::Manifest<IMethodInterface> PluginManifest;

int main(int argc, char *argv[])
{
	PluginLoader loader;

	//std::string path = std::string("/home/stepo/git/videoterror/vtapi_modules/bin/lib") + argv[1] + ".so";
	std::cout << argv[1] << std::endl;

	try
	{
		loader.loadLibrary(argv[1]);
	}
	catch (Poco::Exception &ex)
	{
		std::cout << "Exception message: " << ex.message() << std::endl;
	}

	PluginLoader::Iterator it(loader.begin());
	PluginLoader::Iterator end(loader.end());
	for (; it != end; ++it)
	{
		std::cout << "lib path: " << it->first << std::endl;
		PluginManifest::Iterator itMan(it->second->begin());
		PluginManifest::Iterator endMan(it->second->end());
		for (; itMan != endMan; ++itMan)
			std::cout << itMan->name() << std::endl;
	}

	IMethodInterface* instance = loader.create(loader.begin()->second->begin()->name());
	std::cout << instance->dummy() << std::endl;
	delete instance;
}