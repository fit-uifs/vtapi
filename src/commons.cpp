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

#include <exception>
#include <Poco/Manifest.h>
#include <vtapi/common/global.h>
#include <vtapi/data/commons.h>

using namespace std;

namespace vtapi {


Commons::Commons(const Poco::Util::AbstractConfiguration &config)
{
    _is_owner = true;
    _pconfig = NULL;
    _pbackend = NULL;
    _pconnection = NULL;
    _ploader = NULL;

    try {
        _pconfig = new CONFIG();

        // load config
        loadConfig(config);

        // initialize global logger
        bool ok = Logger::instance().config(_pconfig->logfile, _pconfig->log_errors,
                                            _pconfig->log_warnings, _pconfig->log_debug);
        if (!ok) throw exception();

        // load backend interface + connection
        ok = LoadBackend();
        if (!ok) throw exception();
    }
    catch (...)
    {
        //UnloadBackend();
        vt_destruct(_pconfig);
        throw;
    }
}

Commons::Commons(const Commons& orig, bool new_copy)
{
    _is_owner = new_copy;
    _pconfig = NULL;
    _pbackend = NULL;
    _pconnection = NULL;
    _ploader = NULL;

    try {
        _context = orig._context;

        if (new_copy) {
            // copy config
            _pconfig = new CONFIG(*orig._pconfig);

            // load backend interface + connection
            if (!LoadBackend()) throw exception();
        }
        else {
            _pconfig = orig._pconfig;
            _ploader = orig._ploader;
            _pbackend = orig._pbackend;
            _pconnection = orig._pconnection;
        }
    }
    catch (...)
    {
        if (new_copy) {
            //UnloadBackend();
            vt_destruct(_pconfig);
        }
        throw;
    }
}

bool Commons::LoadBackend()
{
    // get library path
    string lib_name = GetBackendLibName();
    if (lib_name.empty()) return false;

    try {
        // load library
        _ploader = new Poco::ClassLoader<IBackendInterface>();
        _ploader->loadLibrary(lib_name);

        // load plugin interface
        string plugin_name = _ploader->begin()->second->begin()->name();
        _pbackend = _ploader->create(plugin_name);

        // create connection object and connect to database
        _pconnection = _pbackend->createConnection(_pconfig->connection);
        if (!_pconnection->connect()) throw exception();

        return true;
    }
    catch(...) {
        UnloadBackend();
        return false;
    }
}

void Commons::UnloadBackend()
{
    vt_destruct(_pconnection);
    vt_destruct(_pbackend);
    if (_ploader) {
        string lib_name = GetBackendLibName();
        if (!lib_name.empty() && _ploader->isLibraryLoaded(lib_name))
            _ploader->unloadLibrary(lib_name);
        vt_destruct(_ploader);
    }

}

string Commons::GetBackendLibName()
{
    size_t uri_end = _pconfig->connection.find("://");
    if (uri_end != string::npos) {
        string backend_name = _pconfig->connection.substr(0, uri_end);
        return "libvtapi_" + backend_name + Poco::SharedLibrary::suffix();
    }
    else {
        return string();
    }
}

Commons::~Commons()
{
    if (_is_owner) {
        UnloadBackend();
        vt_destruct(_pconfig);
    }
}

Commons::_CONFIG::_CONFIG()
{
    log_errors = false;
    log_warnings = false;
    log_debug = false;
}

Commons::_CONTEXT::_CONTEXT()
{
    process = 0;
}


const Commons::CONFIG& Commons::config()
{
    return *_pconfig;
}


Commons::CONTEXT& Commons::context()
{
    return _context;
}


const IBackendInterface& Commons::backend()
{
    return *_pbackend;
}


Connection& Commons::connection()
{
    return *_pconnection;
}

void Commons::loadConfig(const Poco::Util::AbstractConfiguration &config)
{
    // required properties

    if (config.hasProperty("datasets_dir"))
        _pconfig->datasets_dir = config.getString("datasets_dir");
    else
        throw exception();
    if (config.hasProperty("modules_dir"))
        _pconfig->modules_dir = config.getString("modules_dir");
    else
        throw exception();
    if (config.hasProperty("connection"))
        _pconfig->connection = config.getString("connection");
    else
        throw exception();

    // optional properties

    if (config.hasProperty("config"))
        _pconfig->configfile = config.getString("config");
    if (config.hasProperty("logfile"))
        _pconfig->logfile = config.getString("logfile");
    _pconfig->log_errors = config.hasProperty("log_errors");
    _pconfig->log_warnings = config.hasProperty("log_warnings");
    _pconfig->log_debug = config.hasProperty("log_debug");

    // context properties

    if (config.hasProperty("dataset"))
        _context.dataset = config.getString("dataset");
    if (config.hasProperty("process"))
        _context.process = config.getInt("process");
}

void Commons::saveConfig(Poco::Util::AbstractConfiguration &config)
{
    // required properties

    config.setString("datasets_dir", _pconfig->datasets_dir);
    config.setString("modules_dir", _pconfig->modules_dir);
    config.setString("connection", _pconfig->connection);

    // optional properties

    if (!_pconfig->configfile.empty())
        config.setString("config", _pconfig->configfile);
    if (!_pconfig->logfile.empty())
        config.setString("logfile", _pconfig->logfile);
    if (_pconfig->log_errors)
        config.setBool("log_errors", true);
    if (_pconfig->log_warnings)
        config.setBool("log_warnings", true);
    if (_pconfig->log_debug)
        config.setBool("log_debug", true);

    // context properties

    if (!_context.dataset.empty())
        config.setString("dataset", _context.dataset);
    if (_context.process != 0)
        config.setInt("process", _context.process);
}


}
