/**
 * @file
 * @brief   Methods of Commons class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <exception>
#include <common/vtapi_global.h>
#include <data/vtapi_commons.h>

using namespace std;

namespace vtapi {

Commons::Commons(const gengetopt_args_info& args_info)
{
    // fill configuration
    _config = new CONFIG();
    if (args_info.config_given)     _config->configfile = args_info.config_arg;
    if (args_info.location_given)   _config->baseLocation = args_info.location_arg;
    if (args_info.backend_given)    _config->backend = BackendFactory::type(args_info.backend_arg);
    if (args_info.connection_given) _config->connection = args_info.connection_arg;
    if (args_info.dbfolder_given)   _config->dbfolder = args_info.dbfolder_arg;
    if (args_info.querylimit_given) _config->queryLimit = args_info.querylimit_arg;
    if (args_info.arraylimit_given) _config->arrayLimit = args_info.arraylimit_arg;
    if (args_info.log_given)        _config->logfile = args_info.log_arg;
    _config->verbose = args_info.verbose_given;
    _config->debug = args_info.debug_given;

    // fill default context
    if (args_info.dataset_given)    _context.dataset = args_info.dataset_arg;
    if (args_info.sequence_given)   _context.sequence = args_info.sequence_arg;
    if (args_info.selection_given)  _context.selection = args_info.selection_arg;
    if (args_info.method_given)     _context.method = args_info.method_arg;
    if (args_info.process_given)    _context.process = args_info.process_arg;
    if (args_info.task_given)       _context.task = args_info.task_arg;

    // initialize global logger
    bool ok = Logger::instance().config(_config->logfile, _config->verbose, _config->debug);
    if (!ok) throw new exception;

    // initialize base for all backend-specific objects
    _backendBase = BackendFactory::createBackendBase(_config->backend);
    ok = _backendBase->base_init();
    if (!ok) throw new exception;

    _connection = BackendFactory::createConnection(_config->backend,
                                                   *_backendBase,
                                                   _config->connection,
                                                   _config->dbfolder);
    ok = _connection->connect();
    if (!ok) throw new exception;

    _is_owner = true;
}

Commons::Commons(const Commons& orig, bool new_copy)
{
    if (new_copy) {
        _config = new CONFIG(*orig._config);
        _context = orig._context;
        
        _backendBase = BackendFactory::createBackendBase(_config->backend);
        bool ok = _backendBase->base_init();
        if (!ok) throw new exception;

        _connection = BackendFactory::createConnection(_config->backend,
                                                      *_backendBase,
                                                      _config->connection,
                                                      _config->dbfolder);
        ok = _connection->connect();
        if (!ok) throw new exception;

        _is_owner = true;
    }
    else {
        _config = orig._config;
        _context = orig._context;
        _backendBase = orig._backendBase;
        _connection = orig._connection;

        _is_owner = false;
    }
}

Commons::~Commons()
{
    if (_is_owner) {
        vt_destruct(_connection);
        vt_destruct(_backendBase);
        vt_destruct(_config);
    }
}

Commons::_CONFIG::_CONFIG()
{
    backend = BackendFactory::BACKEND_UNKNOWN;
    queryLimit = 0;
    arrayLimit = 0;
    verbose = false;
    debug = false;
}

Commons::_CONTEXT::_CONTEXT()
{
    process = 0;
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
