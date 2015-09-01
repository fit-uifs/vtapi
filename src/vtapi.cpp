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

#include <Poco/Exception.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/OptionProcessor.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/queries/predefined.h>
#include <vtapi/vtapi.h>

using namespace std;
using Poco::Util::MapConfiguration;
using Poco::Util::PropertyFileConfiguration;
using Poco::Util::LayeredConfiguration;

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
    // master configuration
    Poco::AutoPtr<LayeredConfiguration> config(new LayeredConfiguration());
    // command line configuration
    Poco::AutoPtr<MapConfiguration> cmd_config(new MapConfiguration());
    // config file configuration
    Poco::AutoPtr<PropertyFileConfiguration> file_config(new PropertyFileConfiguration());

    // add configurations to master
    config->add(cmd_config.get(), -100);
    config->add(file_config.get(), 100);

    // define command line options
    Poco::Util::OptionSet options;
    DEFINE_OPTIONS(options, *cmd_config.get());

    // load all command line options into configuration
    Poco::Util::OptionProcessor opt_proc(options);
    string opt_name, opt_arg;
    for (int i = 1; i < argc; i++) {
        bool ok = opt_proc.process(argv[i], opt_name, opt_arg);
        if (ok)
            cmd_config->setString(opt_name, opt_arg);
        else
            throw BadConfigurationException("invalid option: " + string(argv[i]));
    }

    // check config file (given by argument or default one)
    string config_path;
    if (!cmd_config->hasProperty("config"))
        config_path = Poco::Path::current() + "vtapi.conf";
    else
        config_path = cmd_config->getString("config");

    try
    {
        // load configuration from file if found
        if (Poco::File(config_path).exists()) {
            cmd_config->setString("config", config_path);
            file_config->load(config_path);
        }
    }
    catch (Poco::Exception & e)
    {
        throw BadConfigurationException("failed to load config file: " + e.message());
    }

    _pcommons = make_shared<Commons>(*config.get());
}

VTApi::VTApi(const string& config_file)
{
    Poco::AutoPtr<PropertyFileConfiguration> file_config(new PropertyFileConfiguration());
    try
    {
        file_config->load(config_file);
    }
    catch (Poco::Exception & e)
    {
        throw BadConfigurationException("failed to load config file: " + e.message());
    }

    _pcommons = make_shared<Commons>(*file_config);
}

VTApi::VTApi(const VTApi& orig)
    : _pcommons(make_shared<Commons>(*orig._pcommons, true))
{
}

Dataset* VTApi::createDataset(const string& name,
                              const string& location,
                              const string& friendly_name,
                              const string& description) const
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
                            const TaskKeyDefinitions & keys_definition,
                            const TaskParamDefinitions & params_definition,
                            const string& description) const
{
    Method *m = NULL;

    QueryMethodCreate q(*_pcommons, name, keys_definition, params_definition, description);
    if (q.execute()) {
        m = loadMethods(name);
        if (!m->next()) vt_destruct(m);
    }

    return m;
}

Dataset* VTApi::loadDatasets(const string& name) const
{
    return (new Dataset(*_pcommons, name));
}

Method* VTApi::loadMethods(const string& name) const
{
    return (new Method(*_pcommons, name));
}

Sequence* VTApi::loadSequences(const string& name) const
{
    return (new Sequence(*_pcommons, name));
}

Video* VTApi::loadVideos(const string& name) const
{
    return (new Video(*_pcommons, name));
}

ImageFolder* VTApi::loadImageFolders(const string& name) const
{
    return (new ImageFolder(*_pcommons, name));
}

Task* VTApi::loadTasks(const string& name) const
{
    return (new Task(*_pcommons, name));
}

Process* VTApi::loadProcesses(int id) const
{
    return (new Process(*_pcommons, id));
}

bool vtapi::VTApi::deleteDataset(const string &dsname) const
{
    return QueryDatasetDelete(*_pcommons, dsname).execute();
}

bool vtapi::VTApi::deleteMethod(const string &mtname) const
{
    return QueryMethodDelete(*_pcommons, mtname).execute();
}

Process *VTApi::getRunnableProcess() const
{
    Process *prs = NULL;

    if (!_pcommons->_context.dataset.empty() && _pcommons->_context.process != 0) {
        Dataset ds(*_pcommons);
        if (ds.next()) {
            prs = ds.loadProcesses();
            if (!prs->next()) vt_destruct(prs);
        }
    }

    return prs;
}


}
