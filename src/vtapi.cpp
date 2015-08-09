/**
 * @file
 * @brief   Methods of VTApi class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <exception>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/OptionProcessor.h>
#include <Poco/Path.h>
#include <vtapi/common/global.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/vtapi.h>

using namespace std;
using Poco::Util::MapConfiguration;
using Poco::Util::PropertyFileConfiguration;

namespace vtapi {


#define ADD_OPTION(set, cfg, opt, desc) \
    set.addOption(Poco::Util::Option(opt, "", desc).binding(opt, &cfg));
#define ADD_OPTION_ARG(set, cfg, opt, arg, desc) \
    set.addOption(Poco::Util::Option(opt, "", desc).argument(arg).binding(opt, &cfg));
#define DEFINE_OPTIONS(opts, cfg) \
    ADD_OPTION_ARG(opts, cfg, "config", "file", "configuration file path");\
    ADD_OPTION_ARG(opts, cfg, "datasets_dir", "dir", "folder containing your datasets");\
    ADD_OPTION_ARG(opts, cfg, "modules_dir", "dir", "folder containing module binaries");\
    ADD_OPTION_ARG(opts, cfg, "dataset", "name", "default dataset");\
    ADD_OPTION_ARG(opts, cfg, "process", "id", "run VTApi as module instance");\
    ADD_OPTION_ARG(opts, cfg, "connection", "string", "database connection string");\
    ADD_OPTION_ARG(opts, cfg, "logfile", "file", "log file location");\
    ADD_OPTION(opts, cfg, "log_errors", "log error messages");\
    ADD_OPTION(opts, cfg, "log_warnings", "log warning messages");\
    ADD_OPTION(opts, cfg, "log_debug", "log debug messages");


VTApi::VTApi(int argc, char** argv)
{
    _pcommons = NULL;

    Poco::Util::OptionSet options;
    Poco::AutoPtr<MapConfiguration> cmd_config(new MapConfiguration());

    // define command line options
    DEFINE_OPTIONS(options, *cmd_config);

    // load all command line options into configuration
    Poco::Util::OptionProcessor opt_proc(options);
    string opt_name, opt_arg;
    for (int i = 1; i < argc; i++) {
        bool ok = opt_proc.process(argv[i], opt_name, opt_arg);
        if (!ok) throw exception();
    }

    // set default config file
    if (!cmd_config->hasProperty("config"))
        cmd_config->setString("config", Poco::Path::current() + "vtapi.conf");

    // load configuration from config file
    Poco::AutoPtr<PropertyFileConfiguration> file_config(
                new PropertyFileConfiguration(cmd_config->getString("config")));

    // override config file with command line
    string opt_raw;
    MapConfiguration::Keys keys;
    cmd_config->keys(keys);
    for (auto const &key : keys) {
        cmd_config->getRawString(key, opt_raw);
        file_config->setString(key, opt_raw);
    }

    _pcommons = new Commons(*file_config);
}

VTApi::VTApi(const string& configFile)
{
    _pcommons = NULL;

    Poco::AutoPtr<PropertyFileConfiguration> file_config(
                new PropertyFileConfiguration(configFile));
    _pcommons = new Commons(*file_config);
}

VTApi::VTApi(const VTApi& orig)
{
    _pcommons = NULL;
    _pcommons =  new Commons(*orig._pcommons, true);
}


VTApi::~VTApi()
{
    vt_destruct(_pcommons);
}


Dataset* VTApi::createDataset(const string& name,
                              const string& location,
                              const string& friendly_name,
                              const string& description)
{
    Dataset *ds = NULL;

    QueryDatasetCreate q(*_pcommons, name, location, friendly_name, description);
    if (q.execute()) {
        ds = loadDatasets(name);
        if (!ds->next()) vt_destruct(ds);
    }

    return ds;
}

Method* VTApi::createMethod(const string& name,
                            const MethodKeys keys_definition,
                            const MethodParams params_definition,
                            const string& description)
{
    Method *m = NULL;

    QueryMethodCreate q(*_pcommons, name, keys_definition, params_definition, description);
    if (q.execute()) {
        m = loadMethods(name);
        if (!m->next()) vt_destruct(m);
    }

    return m;
}

Dataset* VTApi::loadDatasets(const string& name)
{
    return (new Dataset(*_pcommons, name));
}

Method* VTApi::loadMethods(const string& name)
{
    return (new Method(*_pcommons, name));
}

Sequence* VTApi::loadSequences(const string& name)
{
    return (new Sequence(*_pcommons, name));
}

Video* VTApi::loadVideos(const string& name)
{
    return (new Video(*_pcommons, name));
}

ImageFolder* VTApi::loadImageFolders(const string& name)
{
    return (new ImageFolder(*_pcommons, name));
}

Task* VTApi::loadTasks(const string& name)
{
    return (new Task(*_pcommons, name));
}

Process* VTApi::loadProcesses(int id)
{
    return (new Process(*_pcommons, id));
}


}
