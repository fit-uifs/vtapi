/**
 * @file    commons.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Commons class
 */

#include <vtapi_global.h>
#include <backends/vtapi_backendfactory.h>
#include <common/vtapi_misc.h>
#include <data/vtapi_commons.h>

using std::string;
using std::ifstream;

using namespace vtapi;

Commons::Commons(const Commons& orig) {
    thisClass       = "Commons(copy)";

    libLoader       = orig.libLoader;
    logger          = orig.logger;
    connection      = orig.connection;
    dbconn          = orig.dbconn;
    backend         = orig.backend;
    typeManager     = orig.typeManager;
    fmap            = orig.fmap;

    user            = orig.user;
    format          = orig.format;
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

    // initialize factory (backend_arg is required)
    g_BackendFactory.initialize(string(args_info.backend_arg));
    backend         = g_BackendFactory.backend;
    // get connection string (postgres) or databases folder (sqlite)
    switch (backend) {
        case POSTGRES:
            dbconn  = string(args_info.connection_arg);
            break;
        case SQLITE:
            dbconn  = string(args_info.dbfolder_arg);
            break;
        default:
            dbconn  = string("");
            break;
    }
    // initialize logger (log_arg has default value)
    logger          = new Logger(string(args_info.log_arg), args_info.verbose_given);
    // link libraries and load functions into fmap
    libLoader       = g_BackendFactory.createLibLoader(logger);
    fmap            = libLoader->loadLibs();
    // initialize connection and type managing
    connection      = fmap ? g_BackendFactory.createConnection(fmap, dbconn, logger) : NULL;
    typeManager     = fmap ? g_BackendFactory.createTypeManager(fmap, connection, logger) : NULL;
    g_typeManager   = (void *)typeManager; // global kvuli pg_enum_* handlerum

    // other args (see vtapi.conf)
    dataset         = args_info.dataset_given   ? string(args_info.dataset_arg)     : string ("");
    sequence        = args_info.sequence_given  ? string(args_info.sequence_arg)    : string ("");
    method          = args_info.method_given    ? string(args_info.method_arg)      : string ("");
    process         = args_info.process_given   ? string(args_info.process_arg)     : string ("");
    selection       = args_info.selection_given ? string(args_info.selection_arg)   : string ("");

    user            = args_info.user_given      ? string(args_info.user_arg)        : string("");
    format          = mapFormat(args_info.format_arg);
    input           = args_info.input_given     ? string(args_info.input_arg)       : string("");
    output          = args_info.output_given    ? string(args_info.output_arg)      : string("");
    queryLimit      = args_info.querylimit_given ? args_info.querylimit_arg         : 0;
    arrayLimit      = args_info.arraylimit_given ? args_info.arraylimit_arg         : 0;
    
    baseLocation    = args_info.location_given  ? string(args_info.location_arg)    : string("");

    doom            = true;             // destruct it with fire

#ifdef HAVE_POSTGIS
    initGEOS(geos_notice, geos_notice); // initialize GEOS stuff
#endif
}

Commons::~Commons() {
    if (doom) {
        vt_destruct(typeManager);
        vt_destruct(connection);
        vt_destruct(fmap);
        vt_destruct(libLoader);        
        vt_destruct(logger);
        
#ifdef HAVE_POSTGIS
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
         g_BackendFactory.backend == UNKNOWN ||
        !libLoader || !libLoader->isLoaded() || !fmap ||
        !connection ||
        !typeManager ||
        dbconn.empty());
}


// static
bool Commons::fileExists(const string& filename) {
    ifstream ifile(filename.c_str());
    return ifile.good();
    // FIXME: tady to psalo <incomplete_type>
}

format_t Commons::mapFormat(const string& format) {
    if (format.compare("standard") == 0)    return STANDARD;
    else if (format.compare("csv") == 0)    return CSV;
    else if (format.compare("html") == 0)   return HTML;
    else                                    return STANDARD;
}

