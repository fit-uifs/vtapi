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
#include <common/vtapi_global.h>
#include <common/vtapi_settings.h>
#include <queries/vtapi_predefined_queries.h>
#include <vtapi.h>

using namespace std;

namespace vtapi {


VTApi::VTApi(int argc, char** argv)
{
    gengetopt_args_info args_info;

    // Initialize parser parameters structure
    // Hold check for required arguments until config file is parsed
    struct cmdline_parser_params cli_params;
    cmdline_parser_params_init(&cli_params);
    cli_params.check_required = 0;
    cli_params.print_errors = 0;

    // Parse cmdline first
    bool ok = (cmdline_parser_ext (argc, argv, &args_info, &cli_params) == 0);
    if (ok && args_info.config_given) {
        // Get the rest of arguments from config file, don't override cmdline
        cli_params.initialize = 0;
        cli_params.override = 0;
        cli_params.check_required = 1;

        // Parse config file now
        ok = (cmdline_parser_config_file (args_info.config_arg, &args_info, &cli_params) == 0);
    }
    
    // Check if all required args are specified
    if (cmdline_parser_required (&args_info, "VTApi") != EXIT_SUCCESS) {
        VTLOG_ERROR("Missing required config arguments (possibly connection to DB). Use \"-h\" for help");
        cmdline_parser_free (&args_info);
        throw new exception;
    }

    // Create commons class to store connection etc.
    _commons = new Commons(args_info);
    if (!ok) VTLOG_WARNING("Error parsing config arguments");
    
    cmdline_parser_free (&args_info);
}

VTApi::VTApi(const string& configFile)
{
    gengetopt_args_info args_info;

    // Initialize parser parameters structure
    // Hold check for required arguments until config file is parsed
    struct cmdline_parser_params cli_params;
    cmdline_parser_params_init(&cli_params);
    cli_params.print_errors = 0;
    
    // Parse config file
    bool ok = (cmdline_parser_config_file (configFile.c_str(), &args_info, &cli_params) == 0);

    // Check if all required args are specified
    if (cmdline_parser_required (&args_info, "VTApi") != EXIT_SUCCESS) {
        VTLOG_ERROR("Missing required config arguments (possibly connection to DB). Use \"-h\" for help");
        cmdline_parser_free (&args_info);
        throw new exception;
    }
    
    // Create commons class to store connection etc.
    _commons = new Commons(args_info);
    if (!ok) VTLOG_WARNING("Error parsing config arguments");
    
    cmdline_parser_free (&args_info);
}

VTApi::VTApi(const VTApi& orig)
    : _commons(new Commons(*orig._commons, true))
{ }


VTApi::~VTApi()
{
    vt_destruct(_commons);
}


Dataset* VTApi::createDataset(const string& name,
                              const string& location,
                              const string& friendly_name,
                              const string& description)
{
    Dataset *ds = NULL;

    QueryDatasetCreate q(*_commons, name, location, friendly_name, description);
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

    QueryMethodCreate q(*_commons, name, keys_definition, params_definition, description);
    if (q.execute()) {
        m = loadMethods(name);
        if (!m->next()) vt_destruct(m);
    }

    return m;
}

Dataset* VTApi::loadDatasets(const string& name)
{
    return (new Dataset(*_commons, name));
}

Method* VTApi::loadMethods(const string& name)
{
    return (new Method(*_commons, name));
}

Process *VTApi::initProcess(ProcessState &initState)
{
    Process * p = NULL;

    if (!_commons->_context.dataset.empty() && _commons->_context.process != 0) {
        if (p = new Process(*_commons)) {
            if (p->next()) {
                ProcessState *ps = p->getState();
                if (ps) {
                    initState = *ps;
                    delete ps;
                }
            }
            else {
                vt_destruct(p);
            }
        }
    }

    return p;
}


}
