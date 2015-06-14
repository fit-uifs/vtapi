/**
 * @file
 * @brief   Methods of Commons class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <common/vtapi_global.h>
#include <data/vtapi_commons.h>

using namespace std;

namespace vtapi {


Commons::Commons(const Commons& orig) {
    thisClass       = "Commons(copy)";

    backendBase     = orig.backendBase;
    connection      = orig.connection;
    logger          = orig.logger;
    
    configfile      = orig.configfile;
    backend         = orig.backend;
    dbconn          = orig.dbconn;
    user            = orig.user;
    input           = orig.input;
    output          = orig.output;
    baseLocation    = orig.baseLocation;
    queryLimit      = orig.queryLimit;
    arrayLimit      = orig.arrayLimit;

    dataset         = orig.dataset;
    datasetLocation = orig.datasetLocation;
    sequence        = orig.sequence;
    sequenceLocation= orig.sequenceLocation;

    method          = orig.method;
    process         = orig.process;
    selection       = orig.selection;
    
    doom            = false;            // don't destruct everything
}

Commons::Commons(const gengetopt_args_info& args_info) {    
    thisClass       = "Commons(init)";

    // initialize logger (log_arg has default value)
    logger          = new Logger(string(args_info.log_arg), args_info.verbose_given, args_info.debug_given);

    // other args (see vtapi.conf)
    configfile      = args_info.config_arg;
    dataset         = args_info.dataset_given   ? string(args_info.dataset_arg)     : string ("");
    sequence        = args_info.sequence_given  ? string(args_info.sequence_arg)    : string ("");
    method          = args_info.method_given    ? string(args_info.method_arg)      : string ("");
    process         = args_info.process_given   ? string(args_info.process_arg)     : string ("");
    selection       = args_info.selection_given ? string(args_info.selection_arg)   : string ("");

    user            = args_info.user_given      ? string(args_info.user_arg)        : string("");
    input           = args_info.input_given     ? string(args_info.input_arg)       : string("");
    output          = args_info.output_given    ? string(args_info.output_arg)      : string("");
    queryLimit      = args_info.querylimit_given ? args_info.querylimit_arg         : 0;
    arrayLimit      = args_info.arraylimit_given ? args_info.arraylimit_arg         : 0;
    
    baseLocation    = args_info.location_given  ? string(args_info.location_arg)    : string("");

    // backend_arg is required
    backend         = BackendFactory::type(args_info.backend_arg);
    
    // get connection string (postgres) or databases folder (sqlite)
    switch (backend) {
        case BackendFactory::BACKEND_POSTGRES:
            dbconn = string(args_info.connection_arg);
            break;
        case BackendFactory::BACKEND_SQLITE:
            dbconn = string(args_info.dbfolder_arg);
            break;
        default: break;
    }
    
    // initialize backend objects specific common object
    backendBase     = BackendFactory::createBackendBase(backend, logger);
    if (backendBase && backendBase->base_init()) {
        // initialize connection object and connect
        connection = BackendFactory::createConnection(backend, *backendBase, dbconn);
        if (connection) {
            connection->connect();
        }
    }
    
    doom            = true;             // destruct it with fire

#if HAVE_POSTGIS
    initGEOS(geos_notice, geos_notice); // initialize GEOS stuff
#endif
}

Commons::~Commons() {
    if (doom) {
        vt_destruct(connection);
        vt_destruct(backendBase);
        vt_destruct(logger);
        
#if HAVE_POSTGIS
        finishGEOS();
#endif
    }
}

Connection* Commons::getConnection() {
    return connection;
} 

Logger* Commons::getLogger() {
    return logger;
}

string Commons::getDataset() {
    if (dataset.empty()) logger->warning(153, "No dataset specified", thisClass+"::getDataset()");
    return (dataset);
}

string Commons::getSequence() {
    if (sequence.empty()) logger->warning(154, "No sequence specified", thisClass+"::getSequence()");
    return (sequence);
}

string Commons::getSelection() {
    if (selection.empty()) logger->warning(155, "No selection specified", thisClass+"::getSelection()");
    return selection;
}

string Commons::getProcess() {
    if (process.empty()) logger->warning(161, "No process specified", thisClass+"::getProcess()");
    return process;
}

string Commons::getBaseLocation() {
    if (baseLocation.empty()) logger->warning(156, "No base location specified", thisClass+"::getBaseLocation()");
    return baseLocation;
}

string Commons::getDatasetLocation() {
    if (datasetLocation.empty()) logger->warning(157, "No dataset location specified", thisClass+"::getDatasetLocation()");
    return datasetLocation;
}

string Commons::getSequenceLocation() {
    if (sequenceLocation.empty()) logger->warning(158, "No sequence location specified", thisClass+"::getSequenceLocation()");
    return sequenceLocation;
}

string Commons::getDataLocation() {
    if (baseLocation.empty()) logger->warning(159, "No base location specified", thisClass+"::getDataLocation()");
    if (datasetLocation.empty()) logger->warning(159, "No dataset location specified", thisClass+"::getDataLocation()");
    if (sequenceLocation.empty()) logger->warning(159, "No sequence location specified", thisClass+"::getDataLocation()");
    return (baseLocation + datasetLocation + sequenceLocation);
}

string Commons::getUser() {
    if (user.empty()) logger->warning(160, "No user specified", thisClass+"::getUser()");
    return user;
}

bool Commons::checkCommonsObject() {
    return !(
         backend != BackendFactory::BACKEND_UNKNOWN ||
        !backendBase || !backendBase->base_is_valid() ||
        !connection || dbconn.empty());
}


// static
bool Commons::fileExists(const string& filepath)
{
    struct stat info;
    return (stat(filepath.c_str(), &info) == 0 && info.st_mode & S_IFREG);
}

// static
bool Commons::dirExists(const string& dirpath)
{
    struct stat info;
    return (stat(dirpath.c_str(), &info) == 0 && info.st_mode & S_IFDIR);
}

}
